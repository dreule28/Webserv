#pragma once

#include "webserv.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		int port;
		std::string host;
		std::string index;
		std::string root;

		std::vector<LocationConfig> locations;
};