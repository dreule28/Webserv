#include "socket/Connection_class.hpp"
#include "socket/sockets.hpp"
#include "HttpResponse.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctime>

// Handle reading CGI stdout (non-blocking)
void handle_cgi_stdout(Connection &con) {
	char buf[4096];
	ssize_t bytes = read(con._cgi_stdout_fd, buf, sizeof(buf));

	if (bytes > 0) {
		// Append data to output buffer
		con._cgi_output.append(buf, bytes);
	} else if (bytes == 0) {
		// EOF - CGI finished writing
		std::cout << GREEN << "CGI stdout closed, finalizing response" << RESET << std::endl;
		close(con._cgi_stdout_fd);
		con._cgi_stdout_fd = -1;

		// Wait for child process
		int status;
		waitpid(con._cgi_pid, &status, WNOHANG);

		// Check if process exited successfully
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			con._cgi_state = CGI_ERROR;
			con._write_buffer = errorResponse(500);
		} else {
			// Parse CGI output and build response
			HttpResponse cgi_response(200);
			int result = finalizeCgi(con, cgi_response);
			if (result == 200) {
				con._write_buffer = cgi_response.build();
				con._cgi_state = CGI_DONE;
			} else {
				con._cgi_state = CGI_ERROR;
				con._write_buffer = errorResponse(result);
			}
		}

		// Switch to POLLOUT to send response
		con._poll_fd.events = POLLOUT;
	} else {
		// bytes == -1
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data available, try again later
			return;
		} else {
			// Read error
			std::cerr << RED << "CGI stdout read error: " << strerror(errno) << RESET << std::endl;
			close(con._cgi_stdout_fd);
			con._cgi_stdout_fd = -1;
			con._cgi_state = CGI_ERROR;
			con._write_buffer = errorResponse(500);
			con._poll_fd.events = POLLOUT;

			// Kill CGI process
			kill(con._cgi_pid, SIGKILL);
			waitpid(con._cgi_pid, NULL, WNOHANG);
		}
	}
}

// Handle writing to CGI stdin (non-blocking)
void handle_cgi_stdin(Connection &con) {
	if (con._cgi_stdin_written >= con._fullReq._body.size()) {
		// All data written, close stdin
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
			// All data written
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
		}
	} else if (bytes == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// Can't write now, try again later
			return;
		} else {
			// Write error
			std::cerr << RED << "CGI stdin write error: " << strerror(errno) << RESET << std::endl;
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
			con._cgi_state = CGI_ERROR;

			// Kill CGI process
			kill(con._cgi_pid, SIGKILL);
			waitpid(con._cgi_pid, NULL, WNOHANG);

			// Close stdout too
			if (con._cgi_stdout_fd != -1) {
				close(con._cgi_stdout_fd);
				con._cgi_stdout_fd = -1;
			}

			con._write_buffer = errorResponse(500);
			con._poll_fd.events = POLLOUT;
		}
	}
}

// Check for CGI timeout
void check_cgi_timeout(Connection &con) {
	if (con._cgi_state != CGI_RUNNING)
		return;

	time_t now = time(NULL);
	if (now - con._cgi_start_time > CGI_TIMEOUT) {
		std::cerr << RED << "CGI timeout, killing process" << RESET << std::endl;

		// Kill the CGI process
		kill(con._cgi_pid, SIGKILL);
		waitpid(con._cgi_pid, NULL, WNOHANG);

		// Close pipes
		if (con._cgi_stdout_fd != -1) {
			close(con._cgi_stdout_fd);
			con._cgi_stdout_fd = -1;
		}
		if (con._cgi_stdin_fd != -1) {
			close(con._cgi_stdin_fd);
			con._cgi_stdin_fd = -1;
		}

		con._cgi_state = CGI_ERROR;
		con._write_buffer = errorResponse(504);  // Gateway Timeout
		con._poll_fd.events = POLLOUT;
	}
}
