#include "Config/HttpRequest.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <map>

void HttpRequest::parseReqline(HttpRequest& req, const std::string& rawReq) {
	std::stringstream ss(rawReq);
	std::string methodStr;
	std::string targetStr;
	std::string versionStr;

	if (!(ss >> methodStr >> targetStr >> versionStr))
		throw std::runtime_error("Invalid request line");
	req._method = (methodStr == "GET") ? GET
				: (methodStr == "POST") ? POST
				: (methodStr == "DELETE") ? DELETE
				: UNKNOWN;
	req._target = targetStr;
	req._version = versionStr;

	size_t pos = req._target.find('?');
	if (pos != std::string::npos) {
		req._path = req._target.substr(0, pos);
		req._query = req._target.substr(pos + 1);
	} else {
		req._path = req._target;
		req._query = "";
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
	}
}

void HttpRequest::parseChunked(HttpRequest& req, const std::string& body) {
	size_t pos = 0;

	while (pos < body.size()) {
		size_t lineEnd = body.find("\r\n", pos);
		if (lineEnd == std::string::npos)
			break;
		size_t chunkSize = std::strtoul(body.c_str() + pos, nullptr, 16);
		pos = lineEnd + 2;

		if (chunkSize == 0)
			break;
		if (pos + chunkSize > body.size())
			throw std::runtime_error("Incomplete chunked body");

		req._body += body.substr(pos, chunkSize);
		pos += chunkSize + 2;
	}
	req._isChunked = true;
	req._contentLength = req._body.size();
}

void HttpRequest::stripMultipart(HttpRequest& req) {
	if (!req.headers.count("content-type"))
		return;
	const std::string &ct = req.headers["content-type"];
	size_t bpos = ct.find("boundary=");
	if (bpos == std::string::npos)
		return;
	std::string boundary = "--" + ct.substr(bpos + 9);
	size_t part_header_end = req._body.find("\r\n\r\n");
	if (part_header_end == std::string::npos)
		return;
	std::string content = req._body.substr(part_header_end + 4);
	std::string closing = "\r\n" + boundary + "--";
	size_t tail = content.find(closing);
	if (tail != std::string::npos)
		content.erase(tail);
	req._body = content;
}

void HttpRequest::parseBody(HttpRequest& req, const std::string& rawReq) {
	size_t bodyStart = rawReq.find("\r\n\r\n");
	if (bodyStart == std::string::npos)
		return;
	std::string body = rawReq.substr(bodyStart + 4);

	if (req.headers.count("transfer-encoding") && req.headers["transfer-encoding"] == "chunked")
		parseChunked(req, body);
	else if (req.headers.count("content-length")) {
		req._contentLength = std::strtol(req.headers["content-length"].c_str(), NULL, 10);
		req._body = body.substr(0, req._contentLength);
	}
}

HttpRequest HttpRequest::parseRequest(const std::string& rawReq) {
	if (rawReq.empty())
		throw std::runtime_error("Error: Empty request");
	HttpRequest req;
	parseReqline(req, rawReq);
	parseHeaders(req, rawReq);
	parseBody(req, rawReq);
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

void HttpRequest::print() const {
	std::cout << "\nMethod: " << methodToString(_method) << std::endl;
	std::cout << "Target: " << _target << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Query: " << _query << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::unordered_map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	std::cout << "Body: " << _body << std::endl;
	std::cout << "Content-Length: " << _contentLength << "\n" << std::endl;
}
