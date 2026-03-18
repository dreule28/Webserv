#include "Config/ConfigParser.hpp"
#include <iostream>

static bool hasTrailingSemicolon(const std::string& token) {
	return !token.empty() && token.find_last_not_of(';') != token.size() - 1;
}

Config ConfigParser::parse(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Cannot open config: " + filename);
	std::stringstream ss;
	std::string line;

	while (std::getline(file, line)) {
		std::size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line.erase(commentPos);
		if (line.find_first_not_of(" \t\r\n") == std::string::npos)
			continue;
		ss << line << '\n';
	}

	Config config;
	std::string token;
	while (ss >> token) {
		if (token == "server") {
			ss >> token;
			parseServer(ss, config);
		}
	}
	return config;
}

void ConfigParser::parseServer(std::stringstream& ss, Config& config) {
	ServerConfig server;
	std::string token;
	while (ss >> token && token != "}") {
		if (token == "listen") {
		ss >> token;
		server.port = std::stoi(stripSemicolon(token));
	}
		else if (token == "host") {
			ss >> token;
			server.host = stripSemicolon(token);
		}
		else if (token == "root") {
			ss >> token;
			server.root = stripSemicolon(token);
		}
		else if (token == "index") {
			ss >> token;
			server.index = stripSemicolon(token);
		}
			else if (token == "location") {
				std::string path;
				ss >> path;
				ss >> token;
				if (token != "{")
					throw std::runtime_error("Expected '{' after location path, got: " + token);
				parseLocation(ss, server, path);
			}
	}
	config.servers.push_back(server);
}

void ConfigParser::parseLocation(std::stringstream& ss, ServerConfig& server, const std::string& path) {
	LocationConfig loc;
	loc.path = path;
	std::string token;

	while (ss >> token && token != "}") {
		if (token == "root") {
			ss >> token;
			loc.root = stripSemicolon(token);
		}
			else if (token == "methods") {
				while (ss >> token) {
					std::string method = stripSemicolon(token);
					if (!method.empty())
						loc.methods.push_back(method);
					if (hasTrailingSemicolon(token))
						break;
				}
			}
		else if (token == "upload_dir") {
			ss >> token;
			loc.uploadDir = stripSemicolon(token);
		}
		else if (token == "cgi_extension") {
			ss >> token;
			loc.cgiExtensions = stripSemicolon(token);
			loc.cgiEnabled = true;
		}
		else if (token == "cgi_path") {
			ss >> token;
			loc.cgiPath = stripSemicolon(token);
			loc.cgiEnabled = true;
		}
		else {
			std::cerr << RED << "[WARN] Unkown location directive: " << token << RESET << std::endl;
			ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	server.locations.push_back(loc);
}

std::string ConfigParser::stripSemicolon(const std::string& s) {
	std::size_t end = s.find_last_not_of(';');
	if (end == std::string::npos)
		return "";
	return s.substr(0, end + 1);
}
