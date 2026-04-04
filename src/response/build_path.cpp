#include "HttpResponse.hpp"
#include "Config/LocationConfig.hpp"
#include <filesystem>

std::string buildRealPath(const LocationConfig *loc, const std::string &request_path, bool &is_dir) {
	// Remove the location prefix from the request path
	std::string relative_path = request_path;
	if (request_path.find(loc->path) == 0) {
		relative_path = request_path.substr(loc->path.length());
	}

	// Combine root with the relative path
	std::string full_path = loc->root;

	// Add separator between root and relative path if needed
	bool root_has_slash = !full_path.empty() && full_path[full_path.length() - 1] == '/';
	bool relative_has_slash = !relative_path.empty() && relative_path[0] == '/';

	if (!root_has_slash && !relative_has_slash && !relative_path.empty()) {
		full_path += '/';
	} else if (root_has_slash && relative_has_slash) {
		relative_path = relative_path.substr(1);
	}

	full_path += relative_path;

	// Check if path is a directory
	if (std::filesystem::is_directory(full_path)) {
		is_dir = true;

		// Ensure directory path ends with /
		if (full_path[full_path.length() - 1] != '/') {
			full_path += '/';
		}

		// Try to append index file if configured
		if (!loc->index.empty()) {
			std::string index_path = full_path + loc->index;
			if (std::filesystem::exists(index_path)) {
				// Index file exists, serve it
				is_dir = false;  // We're returning a file, not a directory
				return index_path;
			}
		}

		// No index file found, return directory path
		// (auto-index will be handled in response() function)
		return full_path;
	}

	is_dir = false;
	return full_path;
}
