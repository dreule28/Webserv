#pragma once

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequest.hpp"

class Config {
	public:
		std::vector<ServerConfig> servers;
};