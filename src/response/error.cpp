#include "HttpResponse.hpp"
#include "color.hpp"
#include <iostream>
#include "status.hpp"

std::string	errorResponse(const int error_code) {
	HttpResponse response(error_code);

	// std::cout << RED << "METHOD NOT ALLOWED" << RESET << std::endl;

	std::string error_message = HTTP_STATUS_MESSAGES.at(error_code);

	response.setHeader("Content-Type", "text/html");

	// Create simple HTML error page
	std::ostringstream body_ss;
	body_ss << "<!DOCTYPE html>\n"
			<< "<html>\n"
			<< "<head><title>" << error_code << " " << error_message << "</title></head>\n"
			<< "<body>\n"
			<< "<h1>" << error_code << " " << error_message << "</h1>\n"
			<< "<hr>\n"
			<< "<p>Webserv/1.0</p>\n"
			<< "</body>\n"
			<< "</html>";

	response.body = body_ss.str();

	std::cerr << RED << "Error: " << error_message << RESET << std::endl;

	return response.build();
}
