#include "Config/ConfigParser.hpp"
#include <iostream>

static const char *LINE_MARKER = "__LINE__";

static std::size_t countTrailingSemicolons(const std::string& token) {
	if (token.empty())
		return 0;
	std::size_t lastNonSemicolon = token.find_last_not_of(';');
	if (lastNonSemicolon == token.size() -1)
		return 0;
	if (lastNonSemicolon == std::string::npos)
		return token.size();
	return token.size() - lastNonSemicolon - 1;
}

static void requireSingleTrailingSemicolon(const std::string& token, std::size_t line, const std::string& directive) {
	std::size_t semicolonCount = countTrailingSemicolons(token);
	if (semicolonCount != 1)
		throw std::runtime_error("Line " + std::to_string(line) + ":invalid value for " + directive + ": " + token);
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
	while (ss >> token) {
		if (token.compare(0, std::strlen(LINE_MARKER), LINE_MARKER) == 0) {
			line = std::strtoul(token.c_str() + std::strlen(LINE_MARKER), NULL, 10);
			skipAndCount(ss, line);
			continue;
		}
		return token;
	}
	return token;
}

Config ConfigParser::parse(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Cannot open config: " + filename);

	std::stringstream ss;
	std::string rawLine;
	std::size_t currentLine = 1;

	while (std::getline(file, rawLine)) {
		std::size_t commentPos = rawLine.find('#');
		if (commentPos != std::string::npos)
			rawLine.erase(commentPos);
		ss << LINE_MARKER << currentLine << ' ' << rawLine << '\n';
		++currentLine;
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
			server.port = std::stoi(stripSemicolon(token));
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
		}
		else if (token == "index") {
			token = nextToken(ss, line);
			requireSingleTrailingSemicolon(token, line, "index");
			server.index = stripSemicolon(token);
		}
		else if (token == "location") {
			std::string path = nextToken(ss, line);
			token = nextToken(ss, line);
			if (token != "{")
				throw std::runtime_error("Line " + std::to_string(line) + ": expected '{' after location path, got: " + token);
			parseLocation(ss, server, path, line);
		}
	}
	config.servers.push_back(server);
}

void ConfigParser::parseLocation(std::stringstream& ss, ServerConfig& server, const std::string& path, std::size_t& line) {
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
		else {
			throw std::runtime_error("Line " + std::to_string(line) + ": unknown location directive: " + token);
		}
	}
	std::string effectiveRoot = loc.root;
	if (effectiveRoot.empty())
		effectiveRoot = server.root;
	if (!effectiveRoot.empty() && !isDirectory(effectiveRoot)) {
		throw std::runtime_error("Line " + std::to_string(line) 
		+ ": root directory does not exist: " + effectiveRoot);
	}

	std::string fullPath = jointPaths(effectiveRoot, loc.path);
	if (!pathExists(fullPath)) {
		throw std::runtime_error("Line " + std::to_string(line)
			+ ": location path does not exist: " + fullPath);
	}
	server.locations.push_back(loc);
}


std::string ConfigParser::stripSemicolon(const std::string& s) {
	std::size_t end = s.find_last_not_of(';');
	if (end == std::string::npos)
		return "";
	return s.substr(0, end + 1);
}
