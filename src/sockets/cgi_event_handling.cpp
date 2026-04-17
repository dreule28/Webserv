#include "socket/Connection_class.hpp"
#include "socket/sockets.hpp"
#include "HttpResponse.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctime>

void handle_cgi_stdout(Connection &con) {
	char buf[4096];
	ssize_t bytes = read(con._cgi_stdout_fd, buf, sizeof(buf));

	if (bytes > 0) {
		con._cgi_output.append(buf, bytes);
	} else if (bytes == 0) {
		std::cout << GREEN << "CGI stdout closed, finalizing response" << RESET << std::endl;
		close(con._cgi_stdout_fd);
		con._cgi_stdout_fd = -1;

		int status;
		waitpid(con._cgi_pid, &status, WNOHANG);

		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			con._cgi_state = CGI_ERROR;
			con._write_buffer = errorResponse(500, "CGI process exited with error");
		} else {
			HttpResponse cgi_response(200);
			int result = finalizeCgi(con, cgi_response);
			if (result == 200) {
				con._write_buffer = cgi_response.build();
				con._cgi_state = CGI_DONE;
			} else {
				con._cgi_state = CGI_ERROR;
				con._write_buffer = errorResponse(result, "CGI response finalization failed");
			}
		}

		con._poll_fd.events = POLLOUT;
	} else {
		if (bytes == -1) {
			return;
		} else {
			std::cerr << RED << "CGI stdout read error: " << RESET << std::endl;
			close(con._cgi_stdout_fd);
			con._cgi_stdout_fd = -1;
			con._cgi_state = CGI_ERROR;
			con._write_buffer = errorResponse(500, "CGI stdout read error: ");
			con._poll_fd.events = POLLOUT;

			kill(con._cgi_pid, SIGKILL);
			waitpid(con._cgi_pid, NULL, WNOHANG);
		}
	}
}

void handle_cgi_stdin(Connection &con) {
	if (con._cgi_stdin_written >= con._fullReq._body.size()) {
		close(con._cgi_stdin_fd);
		con._cgi_stdin_fd = -1;
		return;
	}

	const char *data = con._fullReq._body.c_str() + con._cgi_stdin_written;
	size_t remaining = con._fullReq._body.size() - con._cgi_stdin_written;

	ssize_t bytes = write(con._cgi_stdin_fd, data, remaining);

	if (bytes > 0) {
		con._cgi_stdin_written += bytes;
		if (con._cgi_stdin_written >= con._fullReq._body.size()) {
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
		}
	} else {
		if (bytes == -1) {
			return;
		} else {
			std::cerr << RED << "CGI stdin write error: " << RESET << std::endl;
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
			con._cgi_state = CGI_ERROR;

			kill(con._cgi_pid, SIGKILL);
			waitpid(con._cgi_pid, NULL, WNOHANG);

			if (con._cgi_stdout_fd != -1) {
				close(con._cgi_stdout_fd);
				con._cgi_stdout_fd = -1;
			}

			con._write_buffer = errorResponse(500, "CGI stdin write error: ");
			con._poll_fd.events = POLLOUT;
		}
	}
}

void check_cgi_timeout(Connection &con) {
	if (con._cgi_state != CGI_RUNNING)
		return;

	time_t now = time(NULL);
	if (now - con._cgi_start_time > CGI_TIMEOUT) {
		std::cerr << RED << "CGI timeout, killing process" << RESET << std::endl;

		kill(con._cgi_pid, SIGKILL);
		waitpid(con._cgi_pid, NULL, WNOHANG);

		if (con._cgi_stdout_fd != -1) {
			close(con._cgi_stdout_fd);
			con._cgi_stdout_fd = -1;
		}
		if (con._cgi_stdin_fd != -1) {
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
		}

		con._cgi_state = CGI_ERROR;
		con._write_buffer = errorResponse(504, "CGI timeout for PID: " + std::to_string(con._cgi_pid));
		con._poll_fd.events = POLLOUT;
	}
}
