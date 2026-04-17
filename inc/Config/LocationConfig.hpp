#pragma once

#include <string>
#include <vector>
#include <map>
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
		long int		clientMaxBodySize;

		int			returnCode;
		int			redirectCode;
		std::string	redirectTarget;

		std::map<int, std::string> errorPages;

		std::string uploadDir;
		LocationConfig() : path(""), root(""), cgiEnabled(false), cgiExtensions(""), cgiPath(""),
	                   index(""), auto_index(false), clientMaxBodySize(-1),
	                   returnCode(0), redirectCode(0), redirectTarget(""), uploadDir("") {}
};