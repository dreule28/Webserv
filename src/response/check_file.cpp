#include "response.hpp"

int	checkFile(std::string file_path, std::string method) {
	if (access(file_path.c_str(), F_OK) != 0) 
		return -1;

	if (method == "GET" && access(file_path.c_str(), R_OK) != 0)
		return -1;

	return 0;
}
