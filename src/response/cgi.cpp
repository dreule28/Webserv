#include "Config/HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int	processCgi(HttpRequest &request, const std::string &script_path, HttpResponse &response) {
	int	fds[2];
	if (pipe(fds) == -1)
		return 500;

	pid_t pid = fork();
	if (pid == -1)
		return 500;

	if (pid == 0) {
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		dup2(fds[1], STDERR_FILENO);
		close(fds[1]);

		std::vector<std::string> env_strings;

		// Convert method enum to string
		std::string method_str;
		switch (request._method) {
			case GET: method_str = "GET"; break;
			case POST: method_str = "POST"; break;
			case DELETE: method_str = "DELETE"; break;
			default: method_str = "UNKNOWN"; break;
		}

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

		const char *argv[] = {script_path.c_str(), nullptr};
		execve(script_path.c_str(), (char* const*)argv, env_ptr.data());
		exit(1);
	}

	close(fds[1]);
	std::string output;
	char	buf[1024];
	ssize_t	bytes;
	while ((bytes = read(fds[0], buf, sizeof(buf))) > 0)
		output += std::string(buf, bytes);
	close(fds[0]);

	int status;
	waitpid(pid, &status, 0);

	if (status != 0)
		return 500;

	if (output.empty()) {
		return 500;
	}

	response.body = output;

	return 200;
}
