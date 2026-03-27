#include "methods.hpp"
#include "Config/LocationConfig.hpp"

std::string	get_method(const std::string &file_path, bool is_dir, const HttpRequest &request) {
	if (is_dir) {
		std::ostringstream ss;

		ss << "<!DOCTYPE html>\n"
			<< "<html><body>\n"
			<< "<h1>Directory: " << file_path << "</h1>\n";

			for (const auto & entry : std::filesystem::directory_iterator(file_path))
				ss << "<a href='" << request.path << "/" << entry.path().filename().string() << "'>"
					<< request.path << "/" << entry.path().filename().string() << "</a><br>\n";

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
