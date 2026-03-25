#include "Config/HttpRequest.hpp"

HttpRequest HttpRequest::parseRequest(const std::string& reqBody) {
	if (reqBody.empty())
		throw std::runtime_error("Error: Empty request");

	std::stringstream ss(reqBody);
	std::string token;
	HttpRequest req;
	
	while (ss >> token) {
		std::cout << "Current token: " << token << std::endl;
		req.method = (token == "GET") ? GET 
					: (token == "POST") ? POST 
					: (token == "DELETE") ? DELETE 
					: UNKOWN;
		// break;
	}
	std::cout << "Parsed method: " << req.method << std::endl;
	return req;
}
