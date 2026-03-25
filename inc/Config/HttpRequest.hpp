#pragma once

#include "webserv.hpp"
#include "Config/Config.hpp"

class HttpRequest {
	public:
	enum Methods {GET, POST, DELETE, UNKOWN = -1};

	Methods		method;
	std::string target;
	std::string path;
	std::string query;
	std::string version;

	std::map<std::string, std::string> headers;

	std::string body;

	size_t contentLength;
	bool keepAlive;


	HttpRequest parseRequest(const std::string& req);
	private:
};
