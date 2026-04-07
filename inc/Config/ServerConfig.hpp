#pragma once

#include "webserv.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		int port;
		std::string host;
		std::string index;
		std::string root;
		bool auto_index;
		size_t clientMaxBodySize;

		std::vector<LocationConfig> locations;
		ServerConfig() : port(0), auto_index(false), clientMaxBodySize(1000000) {}
};