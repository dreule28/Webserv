#pragma once

#include <string>
#include <vector>
#include "HttpRequest.hpp"

class LocationConfig {
	public:
		std::string path;

		std::string root;
		std::vector<Methods> methods;

		bool        cgiEnabled;
		std::string cgiExtensions;
		std::string cgiPath;

		std::string	index;
		bool		auto_index;
		size_t		clientMaxBodySize;

		int			returnCode;
		int			redirectCode;
		std::string	redirectTarget;

		std::string uploadDir;
		LocationConfig() : cgiEnabled(false), auto_index(false), clientMaxBodySize(1000000), returnCode(0), redirectCode(0) {}
};