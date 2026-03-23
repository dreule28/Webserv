#pragma once

#include "webserv.hpp"
#include "Config.hpp"

class ConfigParser {
	public:
		Config parse(const std::string& filename);

	private:
		void parseServer(std::stringstream& ss, Config& config, std::size_t& line);
		void parseLocation(std::stringstream& ss, ServerConfig& server, const std::string& path, std::size_t& line);
		static std::string stripSemicolon(const std::string& s);
};
