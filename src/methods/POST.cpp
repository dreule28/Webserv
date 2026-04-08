#include "methods.hpp"
#include <unistd.h>

int	post_method(const std::string &file_path, const std::string &content) {
	// Check if file exists to determine status code
	bool file_exists = (access(file_path.c_str(), F_OK) == 0);

	// If file exists, check if we have write permission
	if (file_exists && access(file_path.c_str(), W_OK) == -1)
		return 403;

	std::ofstream file(file_path, std::ios::app);

	if (!file.is_open()) {
		return 500;
	}

	file << content;
	file.close();

	return file_exists ? 200 : 201;
}
