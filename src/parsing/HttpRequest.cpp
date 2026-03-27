#include "Config/HttpRequest.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

void HttpRequest::parseReqline(HttpRequest& req, const std::string& rawReq) {
	std::stringstream ss(rawReq);
	std::string methodStr;
	std::string targetStr;
	std::string versionStr;

	if (!(ss >> methodStr >> targetStr >> versionStr))
		throw std::runtime_error("Invalid request line");
	req.method = (methodStr == "GET") ? GET
				: (methodStr == "POST") ? POST
				: (methodStr == "DELETE") ? DELETE
				: UNKOWN;
	req.target = targetStr;
	req.version = versionStr;

	size_t pos = req.target.find('?');
	if (pos != std::string::npos) {
		req.path = req.target.substr(0, pos);
		req.query = req.target.substr(pos + 1);
	} else {
		req.path = req.target;
		req.query = "";
	}
}

void HttpRequest::parseHeaders(HttpRequest& req, const std::string& rawReq) {
	size_t headerEnd = rawReq.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return;

	std::string headerPart = rawReq.substr(0, headerEnd);
	std::string bodyPart = rawReq.substr(headerEnd + 4);
	std::stringstream ss(headerPart);
	std::string line;

	std::getline(ss, line);
	while (std::getline(ss, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			throw std::runtime_error("Invalid header format");
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		req.headers[key] = value;
		if (req.headers.count("content-length"))
			req.contentLength = std::atoi(req.headers["content-length"].c_str());
		else
			req.contentLength = 0;
		if (bodyPart.size() < req.contentLength)
			return;
		req.body = bodyPart; //.substr(0, req.contentLength);
	}
}

HttpRequest HttpRequest::parseRequest(const std::string& rawReq) {
	if (rawReq.empty())
		throw std::runtime_error("Error: Empty request");
	HttpRequest req;
	parseReqline(req, rawReq);
	parseHeaders(req, rawReq);
	return req;
}

static const char *methodToString(Methods method) {
	if (method == GET)
		return "GET";
	if (method == POST)
		return "POST";
	if (method == DELETE)
		return "DELETE";
	return "UNKNOWN";
}

Methods HttpRequest::stringToMethod(const std::string& methodStr) {
	if (methodStr == "GET")
		return GET;
	if (methodStr == "POST")
		return POST;
	if (methodStr == "DELETE")
		return DELETE;
	return UNKOWN;
}

void HttpRequest::print() const {
	std::cout << "Method: " << methodToString(method) << std::endl;
	std::cout << "Target: " << target << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Query: " << query << std::endl;
	std::cout << "Version: " << version << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	std::cout << "Body: " << body << std::endl;
	std::cout << "Content-Length: " << contentLength << std::endl;
}
