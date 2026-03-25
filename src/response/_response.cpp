#include "response.hpp"
#include "Config/LocationConfig.hpp"
#include "color.hpp"
#include "methods.hpp"
#include <sstream>

std::string response(const http_request &request, const std::vector<LocationConfig> &locations) {
	const LocationConfig *loc = routeMatching(request.path, locations);
	int status = 200;
	std::stringstream ss;

	if (checkMethod(request.method, loc) == false) {
		return errorResponse(405);
	}

	std::string file_path(buildRealPath(loc, request.path));

	if (request.method == "GET" || request.method == "DELETE") {
		status = checkFile(file_path, request.method);
		if (status != 200) {
			if (status == 404)
				return errorResponse(404);
			else if (status == 403)
				return errorResponse(403);
			else
				return errorResponse(status);
		}
	}

	std::string res;

	if (request.method == "GET") {
		std::string content = GET(file_path);

		ss << "HTTP/1.1 200 OK\r\n";
		ss << "Content-Type: text/html\r\n";
		ss << "Content-Length: " << content.size() << "\r\n";
		ss << "\r\n";
		ss << content;
		res = ss.str();
	}
	else if (request.method == "POST") {
		status = POST(file_path, request.body);
		ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";
		res = ss.str();
	}
	else if (request.method == "DELETE") {
		status = DELETE(file_path);
		ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";
		res = ss.str();
	}

	return res;
}