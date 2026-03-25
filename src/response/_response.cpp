#include "response.hpp"
#include "Config/LocationConfig.hpp"
#include "color.hpp"
#include "methods.hpp"

std::string response(const http_request &request, const std::vector<LocationConfig> &locations) {
	const LocationConfig *loc = routeMatching(request.path, locations);

	if (checkMethod(request.method, loc) == false) {
		std::cerr << RED << "Error: method not allowed!" << RESET << std::endl;
		return "";
	}

	std::string file_path(buildRealPath(loc));

	if (request.method == "GET" || request.method == "DELETE") {
		if (checkFile(file_path, request.method) == -1) {
			std::cerr << RED << "Error: " <<
				"file does not exists or doesn't have the correct permissions"
				<< RESET << std::endl;
			return "";
		}
	}

	std::string res = "";

	if (request.method == "GET") {
		res = GET(file_path);
		if (res.empty()) {
			std::cerr << RED << "Error: failed to read file" << RESET << std::endl;
		}
	}
	else if (request.method == "POST") {
		int status = POST(file_path, request.body);
		std::stringstream ss;
		ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";
		res = ss.str();
	}
	else if (request.method == "DELETE") {
		int status = DELETE(file_path);
		std::stringstream ss;
		ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";
		res = ss.str();
	}

	return res;
}