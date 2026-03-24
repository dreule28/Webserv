#include "response.hpp"
#include "LocationConfig.hpp"
#include "color.hpp"

std::string response(const std::vector<LocationConfig> &locations) {
	const LocationConfig *loc = routeMatching(std::string("/test"), locations);

	if (checkMethod(std::string("GET"), loc) == false) {
		std::cerr << RED << "Error: method not allowed!" << RESET << std::endl;
		return "";
	}

	std::string file_path(buildRealPath(loc));

	if (checkFile(file_path, std::string("GET")) == -1) {
		std::cerr << RED << "Error: " <<
			"file does not exists or doesn't have the correct permissions"
			<< RESET << std::endl;
		return "";
	}

	std::string res = "";
	// TODO: execute method

	return res;
}