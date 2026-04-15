#include "Config/HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "socket/Connection_class.hpp"
#include "color.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctime>
#include <cerrno>
#include <cstring>

// Start CGI process in non-blocking mode
// Returns: 0 if CGI started successfully, error code otherwise
int	processCgi(HttpRequest &request, const std::string &script_path, const std::string &cgi_path, Connection &conn) {
	std::cout << YELLOW << "CGI Debug: script_path=" << script_path << ", cgi_path=" << cgi_path << RESET << std::endl;
	int	stdout_fds[2];
	int	stdin_fds[2];

	if (pipe(stdout_fds) == -1) {
		std::cerr << RED << "CGI 500: stdout pipe failed: " << strerror(errno) << RESET << std::endl;
		return 500;
	}
	if (pipe(stdin_fds) == -1) {
		std::cerr << RED << "CGI 500: stdin pipe failed: " << strerror(errno) << RESET << std::endl;
		return close(stdout_fds[0]), close(stdout_fds[1]), 500;
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(stdout_fds[0]);
		close(stdout_fds[1]);
		close(stdin_fds[0]);
		close(stdin_fds[1]);
		std::cerr << RED << "CGI 500: fork failed: " << strerror(errno) << RESET << std::endl;
		return 500;
	}

	if (pid == 0) {
		// Child process - execute CGI script
		close(stdout_fds[0]);
		dup2(stdout_fds[1], STDOUT_FILENO);
		dup2(stdout_fds[1], STDERR_FILENO);
		close(stdout_fds[1]);

		close(stdin_fds[1]);
		dup2(stdin_fds[0], STDIN_FILENO);
		close(stdin_fds[0]);

		std::vector<std::string> env_strings;

		// Convert method enum to string
		std::string method_str;
		switch (request._method) {
			case GET: method_str = "GET"; break;
			case POST: method_str = "POST"; break;
			case DELETE: method_str = "DELETE"; break;
			default: method_str = "UNKNOWN"; break;
		}

		env_strings.push_back("QUERY_STRING=" + request._query);
		env_strings.push_back("REQUEST_METHOD=" + method_str);

		std::unordered_map<std::string, std::string>::const_iterator ct_it = request.headers.find("Content-Type");
		if (ct_it != request.headers.end())
			env_strings.push_back("CONTENT_TYPE=" + ct_it->second);

		env_strings.push_back("CONTENT_LENGTH=" + std::to_string(request._body.size()));
		env_strings.push_back("SCRIPT_FILENAME=" + script_path);

		std::vector<char*> env_ptr;
		for (size_t i = 0; i < env_strings.size(); i++) {
			env_ptr.push_back(const_cast<char*>(env_strings[i].c_str()));
		}
		env_ptr.push_back(nullptr);

		std::string script_dir = script_path.substr(0, script_path.find_last_of('/'));
		std::string script_name = script_path.substr(script_path.find_last_of('/') + 1);
		if (chdir(script_dir.c_str()) == -1) {
			std::cerr << "chdir failed: " << strerror(errno) << " (dir: " << script_dir << ")" << std::endl;
			exit(1);
		}

		const char *argv[] = {cgi_path.c_str(), script_name.c_str(), nullptr};
		execve(cgi_path.c_str(), (char* const*)argv, env_ptr.data());
		std::cerr << "execve failed: " << strerror(errno) << " (cgi_path: " << cgi_path << ", script: " << script_name << ")" << std::endl;
		exit(1);
	}

	// Parent process - set up non-blocking I/O
	close(stdout_fds[1]);
	close(stdin_fds[0]);

	// Set pipes to non-blocking mode
	int flags = fcntl(stdout_fds[0], F_GETFL, 0);
	fcntl(stdout_fds[0], F_SETFL, flags | O_NONBLOCK);

	flags = fcntl(stdin_fds[1], F_GETFL, 0);
	fcntl(stdin_fds[1], F_SETFL, flags | O_NONBLOCK);

	// Store CGI state in connection
	conn._cgi_state = CGI_RUNNING;
	conn._cgi_pid = pid;
	conn._cgi_stdout_fd = stdout_fds[0];
	conn._cgi_stdin_fd = stdin_fds[1];
	conn._cgi_stdin_written = 0;
	conn._cgi_start_time = time(NULL);
	conn._cgi_output.clear();
	conn._cgi_script_path = script_path;
	conn._cgi_path = cgi_path;

	return 0;  // Success - CGI is now running
}

// Finalize CGI after all output has been read
// Parses CGI headers and body, populates response
// Returns: HTTP status code
int finalizeCgi(Connection &conn, HttpResponse &response) {
	if (conn._cgi_output.empty()) {
		std::cerr << RED << "CGI 500: empty output from script: " << conn._cgi_script_path << RESET << std::endl;
		return 500;
	}

	// Split CGI output into headers and body at the blank line
	size_t sep_pos = conn._cgi_output.find("\r\n\r\n");
	if (sep_pos == std::string::npos)
		sep_pos = conn._cgi_output.find("\n\n");
	if (sep_pos == std::string::npos) {
		std::cerr << RED << "CGI 500: no header separator in output from: " << conn._cgi_script_path << RESET << std::endl;
		return 500;
	}

	std::string cgi_headers = conn._cgi_output.substr(0, sep_pos);
	response.body = conn._cgi_output.substr(sep_pos + 4);

	// Parse CGI headers line by line
	std::istringstream header_stream(cgi_headers);
	std::string line;

	while (std::getline(header_stream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);

		// Trim leading space from value
		size_t start = value.find_first_not_of(' ');
		if (start != std::string::npos)
			value = value.substr(start);

		// Status header sets the HTTP response code
		if (key == "Status")
			std::istringstream(value) >> response.status;
		else
			response.setHeader(key, value);
	}

	return 200;
}
