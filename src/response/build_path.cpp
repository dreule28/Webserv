#include "HttpResponse.hpp"
#include "Config/LocationConfig.hpp"
#include <filesystem>

std::string buildRealPath(const LocationConfig *loc, const std::string &request_path, bool &is_dir) {
	std::string relative_path = request_path;

	std::string full_path = loc->root;

	bool root_has_slash = !full_path.empty() && full_path[full_path.length() - 1] == '/';
	bool relative_has_slash = !relative_path.empty() && relative_path[0] == '/';

	if (!root_has_slash && !relative_has_slash && !relative_path.empty()) {
		full_path += '/';
	} else if (root_has_slash && relative_has_slash) {
		relative_path = relative_path.substr(1);
	}

	full_path += relative_path;

	if (std::filesystem::is_directory(full_path)) {
		is_dir = true;

		if (full_path[full_path.length() - 1] != '/') {
			full_path += '/';
		}

		if (!loc->index.empty()) {
			std::string index_path = full_path + loc->index;
			if (std::filesystem::exists(index_path)) {
				is_dir = false;
				return index_path;
			}
		}

		return full_path;
	}

	is_dir = false;
	return full_path;
}
