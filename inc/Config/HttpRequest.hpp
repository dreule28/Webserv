#pragma once

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "../Colors.hpp"

typedef enum {
	GET,
	POST,
	DELETE,
	UNKOWN = -1
} Methods;

class HttpRequest {
	public:

	Methods		_method;
	std::string _target;
	std::string _path;
	std::string _query;
	std::string _version;

	std::map<std::string, std::string> _headers;

	std::string _body;

	bool _isChunked;
	size_t _contentLength;
	bool _keepAlive;

	void parseReqline(HttpRequest& req, const std::string& rawReq);
	void parseHeaders(HttpRequest& req, const std::string& rawReq);
	void parseBody(HttpRequest& req, const std::string& rawReq);
	void parseChunked(HttpRequest& req, const std::string& body);
	HttpRequest parseRequest(const std::string& rawReq);
	void print() const;

	static Methods stringToMethod(const std::string& methodStr);
};
