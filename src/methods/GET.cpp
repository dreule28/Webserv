#include "methods.hpp"
#include "Config/LocationConfig.hpp"

std::string	get_method(const std::string &file_path, bool is_dir, const HttpRequest &request) {
	if (is_dir) {
		std::ostringstream ss;

		ss << "<!DOCTYPE html>\n"
			<< "<html><body>\n"
			<< "<h1>Directory: " << file_path << "</h1>\n";

		std::string base_path = request._path;
		if (base_path.empty() || base_path[base_path.length() - 1] != '/') {
			base_path += "/";
		}

		for (const auto & entry : std::filesystem::directory_iterator(file_path)) {
			std::string link = base_path + entry.path().filename().string();
			ss << "<a href='" << link << "'>" << entry.path().filename().string() << "</a><br>\n";
		}

		ss << "</body></html>";

		return ss.str();
	}

	std::ifstream file(file_path);

	if (!file.is_open()) {
		return "";
	}

	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	return ss.str();
}
