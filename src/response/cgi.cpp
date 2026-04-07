#include "Config/HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int	processCgi(HttpRequest &request, const std::string &script_path, const std::string &cgi_path, HttpResponse &response) {
	int	stdout_fds[2];
	int	stdin_fds[2];

	if (pipe(stdout_fds) == -1)
		return 500;
	if (pipe(stdin_fds) == -1)
		return close(stdout_fds[0]), close(stdout_fds[1]), 500;

	pid_t pid = fork();
	if (pid == -1) {
		close(stdout_fds[0]);
		close(stdout_fds[1]);
		close(stdin_fds[0]);
		close(stdin_fds[1]);
		return 500;
	}

	if (pid == 0) {
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
		if (chdir(script_dir.c_str()) == -1)
			exit(1);

		const char *argv[] = {cgi_path.c_str(), script_path.c_str(), nullptr};
		execve(cgi_path.c_str(), (char* const*)argv, env_ptr.data());
		exit(1);
	}

	close(stdout_fds[1]);
	close(stdin_fds[0]);

	// Write POST body to child's stdin
	if (!request._body.empty()) {
		write(stdin_fds[1], request._body.c_str(), request._body.size());
	}
	close(stdin_fds[1]);

	std::string output;
	char	buf[1024];
	ssize_t	bytes;
	while ((bytes = read(stdout_fds[0], buf, sizeof(buf))) > 0)
		output += std::string(buf, bytes);
	close(stdout_fds[0]);

	int status;
	waitpid(pid, &status, 0);

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return 500;

	if (output.empty())
		return 500;


	// Split CGI output into headers and body at the blank line
	size_t sep_pos = output.find("\r\n\r\n");
	if (sep_pos == std::string::npos)
		sep_pos = output.find("\n\n");
	if (sep_pos == std::string::npos)
		return 500;


	std::string cgi_headers = output.substr(0, sep_pos);
	response.body = output.substr(sep_pos + 4);

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
