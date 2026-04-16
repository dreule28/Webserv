#include "Config/ConfigParser.hpp"
#include <iostream>
#include <string>

static std::size_t countTrailingSemicolons(const std::string& token) {
	if (token.empty())
		return 0;
	std::size_t lastNonSemicolon = token.find_last_not_of(';');
	if (lastNonSemicolon == token.size() - 1)
		return 0;
	if (lastNonSemicolon == std::string::npos)
		return token.size();
	return token.size() - lastNonSemicolon - 1;
}

static void requireSingleTrailingSemicolon(const std::string& token, std::size_t line, const std::string& directive) {
	std::size_t semicolonCount = countTrailingSemicolons(token);
	if (semicolonCount != 1)
		throw std::runtime_error("Line " + std::to_string(line) + ": invalid value for " + directive + ": " + token);
}

static void skipAndCount(std::stringstream& ss, std::size_t& line) {
	char c;
	while (ss.peek() != EOF && std::isspace(ss.peek())) {
		ss.get(c);
		if (c == '\n')
			++line;
	}
}

static std::string nextToken(std::stringstream& ss, std::size_t& line) {
	skipAndCount(ss, line);
	std::string token;
	ss >> token;
	return token;
}

static size_t parseBodySize(const std::string& sizeStr, std::size_t line) {
	if (sizeStr.empty())
		throw std::runtime_error("Line " + std::to_string(line) + ": client_max_body_size cannot be empty");
	try {
		char lastChar = sizeStr.back();
		size_t multiplier = 1;
		std::string numStr = sizeStr;
		if (lastChar == 'k' || lastChar == 'K') {
			multiplier = 1024;
			numStr.pop_back();
		} else if (lastChar == 'm' || lastChar == 'M') {
			multiplier = 1024 * 1024;
			numStr.pop_back();
		} else if (lastChar == 'g' || lastChar == 'G') {
			multiplier = 1024 * 1024 * 1024;
			numStr.pop_back();
		}
		size_t size = std::stoul(numStr);
		return size * multiplier;
	} catch (const std::invalid_argument& e) {
		throw std::runtime_error("Line " + std::to_string(line) + ": client_max_body_size must be a number with optional suffix (k/m/g), got: " + sizeStr);
	} catch (const std::out_of_range& e) {
		throw std::runtime_error("Line " + std::to_string(line) + ": client_max_body_size value too large: " + sizeStr);
	}
}

Config ConfigParser::parse(const std::string& filename) {
	if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".conf")
		throw std::runtime_error("Invalid config file: " + filename + " (must end with .conf)");

	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Cannot open config: " + filename);

	std::stringstream ss;
	std::string rawLine;

	while (std::getline(file, rawLine)) {
		std::size_t commentPos = rawLine.find('#');
		if (commentPos != std::string::npos)
			rawLine.erase(commentPos);
		ss << rawLine << '\n';
	}

	Config config;
	std::string token;
	std::size_t line = 1;

	while (!(token = nextToken(ss, line)).empty()) {
		if (token == "server") {
			token = nextToken(ss, line);
			if (token != "{")
				throw std::runtime_error("Line " + std::to_string(line) + ": expected '{' after server, got: " + token);
			parseServer(ss, config, line);
		}
	}
	return config;
}

void ConfigParser::parseServer(std::stringstream& ss, Config& config, std::size_t& line) {
	ServerConfig server;
	std::string token;

	while (!(token = nextToken(ss, line)).empty() && token != "}") {
		if (token == "listen") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "listen");
			server.port = std::strtol(stripSemicolon(token).c_str(), NULL, 10);
		}
		else if (token == "host") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "host");
			server.host = stripSemicolon(token);
		}
		else if (token == "root") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "root");
			server.root = stripSemicolon(token);
			if (!server.root.empty() && !isDirectory(server.root)) {
				throw std::runtime_error("Line " + std::to_string(line) + ": root directory does not exist: " + server.root);
			}
		}
		else if (token == "index") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "index");
			server.index = stripSemicolon(token);
		}
		else if (token == "auto_index") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "auto_index");
			std::string value = stripSemicolon(token);
			if (value == "true") {
				server.auto_index = true;
			} else if (value == "false") {
				server.auto_index = false;
			} else {
				throw std::runtime_error("Line " + std::to_string(line) + ": auto_index must be 'true' or 'false', got: " + value);
			}
		}
		else if (token == "client_max_body_size") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "client_max_body_size");
			server.clientMaxBodySize = parseBodySize(stripSemicolon(token), line);
		}
		else if (token == "error_page") {
			std::string codeStr = nextToken(ss, line);
			int code = std::stoi(codeStr);
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "error_page");
			server.errorPages[code] = stripSemicolon(token);
		}
		else if (token == "location") {
			std::string path = nextToken(ss, line);
			std::size_t locationLine = line;
			token = nextToken(ss, line);
			if (token != "{")
				throw std::runtime_error("Line " + std::to_string(line) + ": expected '{' after location path, got: " + token);
			parseLocation(ss, server, path, line, locationLine);
		}
	}
	if (server.port == 0)
		throw std::runtime_error("Server block missing required 'listen' directive");
	if (server.root.empty())
		throw std::runtime_error("Server block missing required 'root' directive");
	config.servers.push_back(server);
}

void ConfigParser::parseLocation(std::stringstream& ss, ServerConfig& server, const std::string& path, std::size_t& line, std::size_t locationLine) {
	LocationConfig loc;
	loc.path = path;
	std::string token;

	while ((token = nextToken(ss, line)) != "}") {
		if (token.empty())
			break;
		if (token == "root") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "root");
			loc.root = stripSemicolon(token);
		}
		else if (token == "methods") {
			while (!(token = nextToken(ss, line)).empty()) {
				std::size_t semicolonCount = countTrailingSemicolons(token);
				if (semicolonCount > 1)
					throw std::runtime_error("Line " + std::to_string(line) + ": invalid value for methods: " + token);
				std::string method = stripSemicolon(token);
				if (!method.empty())
					loc.methods.push_back(HttpRequest::stringToMethod(method));
				if (loc.methods.back() == -1)
					throw std::runtime_error("Line " + std::to_string(line) + ": invalid method:" + token);
				if (semicolonCount == 1)
					break;
			}
		}
		else if (token == "upload_dir") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "upload_dir");
			loc.uploadDir = stripSemicolon(token);
		}
		else if (token == "cgi_extension") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "cgi_extension");
			loc.cgiExtensions = stripSemicolon(token);
			loc.cgiEnabled = true;
		}
		else if (token == "cgi_path") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "cgi_path");
			loc.cgiPath = stripSemicolon(token);
			loc.cgiEnabled = true;
		}
		else if (token == "index") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "index");
			loc.index = stripSemicolon(token);
		}
		else if (token == "auto_index") {
		token = nextToken(ss, line);
		requireSingleTrailingSemicolon(token, line, "auto_index");
		std::string value = stripSemicolon(token);
		if (value == "true") {
			loc.auto_index = true;
		} else if (value == "false") {
			loc.auto_index = false;
		} else {
			throw std::runtime_error("Line " + std::to_string(line) + ": auto_index must be 'on' or 'off', got: " + value);
		}
		}
		else if (token == "client_max_body_size") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "client_max_body_size");
			loc.clientMaxBodySize = parseBodySize(stripSemicolon(token), line);
		}
		else if (token == "return") {
			std::string code_token = nextToken(ss, line);
			std::string target_token = nextToken(ss, line);
			requireSingleTrailingSemicolon(target_token, line, "return");
			loc.redirectCode = std::stoi(code_token);
			loc.redirectTarget = stripSemicolon(target_token);
		}
		else {
			throw std::runtime_error("Line " + std::to_string(line) + ": unknown location directive: " + token);
		}
	}
	std::string effectiveRoot = loc.root;
	if (effectiveRoot.empty())
		effectiveRoot = server.root;
	if (!effectiveRoot.empty() && !isDirectory(effectiveRoot)) {
		throw std::runtime_error("Line " + std::to_string(locationLine)
			+ ": location " + loc.path + ": root directory does not exist: " + effectiveRoot);
	}

	std::string fullPath = jointPaths(effectiveRoot, loc.path);
	if (!pathExists(fullPath)) {
		throw std::runtime_error("Line " + std::to_string(locationLine)
			+ ": location " + loc.path + ": path does not exist: " + fullPath);
	}
	server.locations.push_back(loc);
}

std::string ConfigParser::stripSemicolon(const std::string& s) {
	std::size_t end = s.find_last_not_of(';');
	if (end == std::string::npos)
		return "";
	return s.substr(0, end + 1);
}