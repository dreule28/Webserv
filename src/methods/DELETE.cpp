#include "methods.hpp"
#include "color.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>

int delete_method(const std::string &file_path) {
	if (!std::filesystem::exists(file_path)) {
		std::cerr << RED << "DELETE 404: file not found: " << file_path << RESET << std::endl;
		return 404;
	}

	if (std::filesystem::remove(file_path))
		return 204;

	std::cerr << RED << "DELETE 403: failed to remove: " << file_path << " (" << strerror(errno) << ")" << RESET << std::endl;
	return 403;
}
