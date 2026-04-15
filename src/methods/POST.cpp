#include "methods.hpp"
#include "color.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>

int	post_method(const std::string &file_path, const std::string &content) {
	// Check if file exists to determine status code
	bool file_exists = (access(file_path.c_str(), F_OK) == 0);

	// If file exists, check if we have write permission
	if (file_exists && access(file_path.c_str(), W_OK) == -1) {
		std::cerr << RED << "POST 403: write permission denied: " << file_path << RESET << std::endl;
		return 403;
	}

	std::ofstream file(file_path, std::ios::app);

	if (!file.is_open()) {
		std::cerr << RED << "POST 500: failed to open file: " << file_path << " (" << strerror(errno) << ")" << RESET << std::endl;
		return 500;
	}

	file << content;
	file.close();

	return file_exists ? 200 : 201;
}
