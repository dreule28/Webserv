#include "response.hpp"
#include <filesystem>

int	checkFile(std::string file_path, std::string method) {
	if (std::filesystem::exists(file_path) == false)
		return 404;
	else if (method == "GET" && access(file_path.c_str(), R_OK) != 0)
		return 403;
	return 200;
}
