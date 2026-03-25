#include "methods.hpp"

int delete_method(const std::string &file_path) {
	if (!std::filesystem::exists(file_path))
		return 404;

	if (std::filesystem::remove(file_path))
		return 204;

	return 403;
}
