#pragma once

#include <string>
#include <vector>

class LocationConfig {
	public:
		std::string path;

		std::string root;
		std::vector<std::string> methods;

		bool        cgiEnabled;
		std::string cgiExtensions;
		std::string cgiPath;

		std::string uploadDir;
		LocationConfig() : cgiEnabled(false) {}
};