#include "Config/ConfigParser.hpp"

bool ConfigParser::pathExists(const std::string& path) {
	return std::filesystem::exists(path);
}

bool ConfigParser::isDirectory(const std::string& path) {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::string ConfigParser::jointPaths(const std::string& root, const std::string& subpath) {
	if (root.empty())
		return subpath;
	if (subpath.empty() || subpath == "/")
		return root;
	
	std::filesystem::path fullPath(root);
	if (!subpath.empty() && subpath[0] == '/')
		fullPath /= subpath.substr(1);
	else
		fullPath /= subpath;
	return fullPath.string();
}

Methods HttpRequest::stringToMethod(const std::string& methodStr) {
	if (methodStr == "GET")
		return GET;
	if (methodStr == "POST")
		return POST;
	if (methodStr == "DELETE")
		return DELETE;
	return UNKOWN;
}
