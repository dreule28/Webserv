#include "HttpResponse.hpp"
#include "socket/Connection_class.hpp"
#include "color.hpp"
#include <iostream>
#include <fstream>
#include "status.hpp"

std::string	errorResponse(const int error_code, const std::string msg) {
	HttpResponse response(error_code);

	std::cerr << RED << msg << RESET << std::endl;

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

	bool customPageLoaded = false;

	if (globalTempConn && !globalTempConn->_serverConfig.errorPages.empty())
	{
		std::map<int, std::string>::const_iterator it = globalTempConn->_serverConfig.errorPages.find(error_code);
		if (it != globalTempConn->_serverConfig.errorPages.end())
		{
			std::ifstream file(it->second.c_str());
			if (file.is_open())
			{
				std::ostringstream custom_body;
				custom_body << file.rdbuf();
				response.body = custom_body.str();
				file.close();
				customPageLoaded = true;
			}
		}
	}

	if (!customPageLoaded)
		response.body = body_ss.str();

	std::cerr << RED << "Error: " << error_message << RESET << std::endl;

	return response.build();
}
