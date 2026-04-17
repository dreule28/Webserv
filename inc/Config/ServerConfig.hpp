#pragma once

#include "webserv.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		int port;
		std::string host;
		std::string server_name;
		std::string index;
		std::string root;
		bool auto_index;
		size_t clientMaxBodySize;
		std::map<int, std::string> errorPages;

		std::vector<LocationConfig> locations;
		ServerConfig() : port(0), host(""), server_name(""), index(""), root(""),
	                 auto_index(false), clientMaxBodySize(1000000) {}
};