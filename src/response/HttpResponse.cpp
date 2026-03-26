#include "HttpResponse.hpp"
#include "status.hpp"
#include "Config/LocationConfig.hpp"
#include "color.hpp"
#include "methods.hpp"
#include <sstream>

std::string getContentType(const std::string &file_path) {
	size_t dot_pos = file_path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return "application/octet-stream";

	std::string ext = file_path.substr(dot_pos);

	std::unordered_map<std::string, std::string>::const_iterator it = MIME_TYPES.find(ext);
	if (it != MIME_TYPES.end())
		return it->second;

	return "application/octet-stream";
}

HttpResponse::HttpResponse(int s) : status(s) {}

HttpResponse::HttpResponse(const HttpResponse &other) : status(other.status), headers(other.headers), body(other.body) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
	if (this != &other) {
		status = other.status;
		headers = other.headers;
		body = other.body;
	}
	return *this;
}


void HttpResponse::setHeader(const std::string &key, const std::string &value) {
	this->headers[key] = value;
}

void HttpResponse::setStandardHeader(void) {
	if (headers.find("Server") == headers.end())
		headers["Server"] = "Webserv/1.0";
	if (headers.find("Connection") == headers.end())
		headers["Connection"] = "close";
	if (!body.empty() && headers.find("Content-Length") == headers.end())
		headers["Content-Length"] = std::to_string(body.size());
}

std::string HttpResponse::build(void) {
	setStandardHeader();

	std::ostringstream ss;
	ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator i = headers.begin();
	i != headers.end(); ++i) {
		ss << i->first << ": " << i->second << "\r\n";
	}

	ss << "\r\n";

	if (!body.empty())
		ss << body;

	return ss.str();
}

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
				return errorResponse(status);
		}
	}

	if (request.method == "GET") {
		HttpResponse response(200);
		response.setHeader("Content-Type", getContentType(file_path));
		response.body = GET(file_path);
		return response.build();
	}
	else if (request.method == "POST") {
		status = POST(file_path, request.body);
		HttpResponse response(status);
		response.setHeader("Content-Type", "application/json");
		response.body = "{\"status\": \"success\"}";
		return response.build();
	}
	else if (request.method == "DELETE") {
		status = DELETE(file_path);
		HttpResponse response(status);
		response.setHeader("Content-Type", "application/json");
		response.body = "{\"status\": \"deleted\"}";
		return response.build();
	}

	return errorResponse(500);
}