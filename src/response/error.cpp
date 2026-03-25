#include "response.hpp"
#include "color.hpp"
#include <iostream>

std::string	errorResponse(const int error_code) {
	std::ostringstream ss;
	std::string body = HTTP_STATUS_MESSAGES.at(error_code);

	ss << "HTTP/1.1 " << error_code << " " <<
		body << "\r\n" <<
		"Content-Type: text/plain\r\n" <<
		"Content-Length: " << body.size() << "\r\n" <<
		"Connection: close\r\n" <<
		"\r\n";

	if (!body.empty())
		ss << body;

	std::cerr << RED << "Error: " << body << RESET << std::endl;

	return ss.str();
}

