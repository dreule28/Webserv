#include "HttpResponse.hpp"
#include "Config/HttpRequest.hpp"
#include "color.hpp"
#include <filesystem>
#include <iostream>
#include <cerrno>
#include <cstring>

int	checkFile(std::string file_path, Methods method) {
	if (std::filesystem::exists(file_path) == false) {
		std::cerr << RED << "404: file not found: " << file_path << RESET << std::endl;
		return 404;
	}
	else if (method == GET && access(file_path.c_str(), R_OK) != 0) {
		std::cerr << RED << "403: read permission denied: " << file_path << RESET << std::endl;
		return 403;
	}
	return 200;
}
