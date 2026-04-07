#pragma once

#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>
#include <unordered_map>
#include "Config/LocationConfig.hpp"
#include "Config/HttpRequest.hpp"

// MIME type mappings (using unordered_map for O(1) lookup)
static const std::unordered_map<std::string, std::string> MIME_TYPES = {
	// Text files
	{".html", "text/html"},
	{".htm", "text/html"},
	{".css", "text/css"},
	{".js", "text/javascript"},
	{".txt", "text/plain"},
	{".xml", "text/xml"},
	{".csv", "text/csv"},

	// Images
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".png", "image/png"},
	{".gif", "image/gif"},
	{".svg", "image/svg+xml"},
	{".ico", "image/x-icon"},
	{".webp", "image/webp"},
	{".bmp", "image/bmp"},

	// Application
	{".json", "application/json"},
	{".pdf", "application/pdf"},
	{".zip", "application/zip"},
	{".tar", "application/x-tar"},
	{".gz", "application/gzip"},
	{".wasm", "application/wasm"},

	// Audio/Video
	{".mp3", "audio/mpeg"},
	{".mp4", "video/mp4"},
	{".avi", "video/x-msvideo"},
	{".wav", "audio/wav"},
	{".ogg", "audio/ogg"},
	{".webm", "video/webm"}
};

class HttpResponse {
	public:
		int status = 200;
		std::map<std::string, std::string> headers;
		std::string body;

		HttpResponse() = default;
		HttpResponse(const int status);
		HttpResponse(const HttpResponse &other);
		HttpResponse &operator=(const HttpResponse &other);
		~HttpResponse() = default;

		void setHeader(const std::string &key, const std::string &value);
		void setStandardHeader(void);
		std::string build(void);
		void	print(void);
};

struct http_request {
	std::string 						method;			// GET, POST, DELETE, etc.
	std::string 						path;			// /api/users
	std::string 						http_version;	// HTTP/1.1
	std::map<std::string, std::string>	headers;		// Host, Content-Type, etc.
	std::string 						body;			// Request body content
};

const LocationConfig	*routeMatching(const std::string &uri, const std::vector<LocationConfig> &locations);
bool	checkMethod(const Methods method, const LocationConfig *location);
std::string buildRealPath(const LocationConfig *loc, const std::string &request_path, bool &is_dir);
int	checkFile(std::string file_path, Methods method);
std::string response(const HttpRequest &request, const std::vector<LocationConfig> &locations);

std::string	errorResponse(const int error_code);
std::string getContentType(const std::string &file_path);

int processCgi(HttpRequest &request, const std::string &script_path, const std::string &cgi_path, HttpResponse &response);

// std::string mock_response(void);
