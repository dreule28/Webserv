#include "HttpResponse.hpp"
#include "Config/HttpRequest.hpp"
#include "status.hpp"
#include "Config/LocationConfig.hpp"
#include "color.hpp"
#include "methods.hpp"
#include <sstream>
#include <chrono>

std::string getContentType(const std::string &file_path) {
	size_t dot_pos = file_path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return "application/octet-stream";

	std::string ext = file_path.substr(dot_pos);

	std::unordered_map<std::string, std::string>::const_iterator it = MIME_TYPES.find(ext);
	if (it != MIME_TYPES.end())
		return it->second;

	return "application/octet-stream";
}

HttpResponse::HttpResponse(int s) : status(s) {}

HttpResponse::HttpResponse(const HttpResponse &other) : status(other.status), headers(other.headers), body(other.body) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
	if (this != &other) {
		status = other.status;
		headers = other.headers;
		body = other.body;
	}
	return *this;
}


void HttpResponse::setHeader(const std::string &key, const std::string &value) {
	this->headers[key] = value;
}

void HttpResponse::setStandardHeader(void) {
	if (headers.find("Server") == headers.end())
		headers["Server"] = "Webserv/1.0";
	if (headers.find("Connection") == headers.end())
		headers["Connection"] = "close";
	if (headers.find("Content-Length") == headers.end())
		headers["Content-Length"] = std::to_string(body.size());
}

void	HttpResponse::print(void) {
	std::cout << "\n=== HTTP Response ===" << std::endl;
	std::cout << "Status: " << status << " " << HTTP_STATUS_MESSAGES.at(status) << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	std::cout << "Body Length: " << body.length() << std::endl;
	if (!body.empty() && body.length() <= 200)
		std::cout << "Body: " << body << std::endl;
	std::cout << "=====================\n" << std::endl;
}

std::string HttpResponse::build(void) {
	setStandardHeader();

	std::ostringstream ss;
	ss << "HTTP/1.1 " << status << " " << HTTP_STATUS_MESSAGES.at(status) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator i = headers.begin();
	i != headers.end(); ++i) {
		ss << i->first << ": " << i->second << "\r\n";
	}

	ss << "\r\n";

	if (!body.empty())
		ss << body;

	// std::cout << ss.str() << std::endl;
	print();

	return ss.str();
}

std::string response(const HttpRequest &request, const std::vector<LocationConfig> &locations) {
	request.print();

	// hard coded max body size
	const ssize_t client_max_body_size = 1 << 16;
	
	const LocationConfig *loc = routeMatching(request._path, locations);
	int status = 200;
	std::stringstream ss;

	// comment out when values are parsed
	// if (loc.redir_code > 0) {
	// 	HttpResponse response(loc.redir_code);
	// 	response.setHeader("Location", loc.redir_target);
	// 	return response.build();
	// }

	if (checkMethod(request._method, loc) == false) {
		return errorResponse(405);
	}

	bool is_dir = false;
	std::string file_path(buildRealPath(loc, request._path, is_dir));

	// Check if CGI is enabled for this location and the file matches the CGI extension
	if (loc && loc->cgiEnabled && !is_dir) {
		size_t dot_pos = file_path.find_last_of('.');
		if (dot_pos != std::string::npos) {
			std::string extension = file_path.substr(dot_pos);
			if (extension == loc->cgiExtensions) {
				HttpResponse cgi_response(200);
				HttpRequest &mutable_request = const_cast<HttpRequest&>(request);
				status = processCgi(mutable_request, file_path, cgi_response);

				if (status == 200) {
					cgi_response.status = status;
					cgi_response.setHeader("Content-Type", "text/html");
					return cgi_response.build();
				}
				return errorResponse(status);
			}
		}
	}

	if (request._method == GET || request._method == DELETE) {
		status = checkFile(file_path, request._method);
		if (status != 200) {
				return errorResponse(status);
		}
	}

	if (request._method == GET) {
		HttpResponse response(200);
		if (is_dir) {
			// Check if auto_index is enabled before showing directory listing
			if (!loc->auto_index) {
				return errorResponse(403);  // Forbidden - directory listing disabled
			}
			response.setHeader("Content-Type", "text/html");
		}
		else {
			response.setHeader("Content-Type", getContentType(file_path));
		}
		response.body = get_method(file_path, is_dir, request);
		return response.build();
	}
	else if (request._method == POST) {
		if (client_max_body_size < request._body.size())
			return errorResponse(413);

		if (!loc->uploadDir.empty()) {
			size_t pos = request._path.find_last_of('/');
			std::string filename = request._path.substr(pos + 1);

			if (filename.empty()) {
				auto now = std::chrono::system_clock::now();
				auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
				filename = "upload_" + std::to_string(timestamp);
			}

			file_path = loc->uploadDir + "/" + filename;
		}
		status = post_method(file_path, request._body);

		HttpResponse response(status);
		response.setHeader("Content-Type", "application/json");

		// Extract filename from file_path to return to client
		size_t pos = file_path.find_last_of('/');
		std::string created_filename = (pos != std::string::npos) ? file_path.substr(pos + 1) : file_path;
		response.body = "{\"status\": \"success\", \"filename\": \"" + created_filename + "\"}";
		return response.build();
	}
	else if (request._method == DELETE) {
		status = delete_method(file_path);

		HttpResponse response(status);
		response.setHeader("Content-Type", "application/json");
		response.body = "{\"status\": \"deleted\"}";
		return response.build();
	}

	return errorResponse(500);
}
