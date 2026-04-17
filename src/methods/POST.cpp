#include "methods.hpp"
#include "color.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>

int	post_method(const std::string &file_path, const std::string &content) {
	bool file_exists = (access(file_path.c_str(), F_OK) == 0);

	if (file_exists && access(file_path.c_str(), W_OK) == -1) {
		std::cerr << RED << "POST 403: write permission denied: " << file_path << RESET << std::endl;
		return 403;
	}

	std::filesystem::path dir = std::filesystem::path(file_path).parent_path();
	if (!dir.empty() && !std::filesystem::exists(dir))
		std::filesystem::create_directories(dir);

	std::ofstream file(file_path, std::ios::trunc);

	if (!file.is_open()) {
		std::cerr << RED << "POST 500: failed to open file: " << file_path << RESET << std::endl;
		return 500;
	}

	file << content;
	file.close();

	return file_exists ? 200 : 201;
}
