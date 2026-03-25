#pragma once

#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>
#include <unordered_map>
#include "Config/LocationConfig.hpp"

// HTTP status codes and their messages (using unordered_map for O(1) lookup)
static const std::unordered_map<int, std::string> HTTP_STATUS_MESSAGES = {
	// 1xx: Informational
	{100, "Continue"},
	{101, "Switching Protocols"},
	{102, "Processing"},
	{103, "Early Hints"},

	// 2xx: Successful
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},

	// 3xx: Redirection
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},

	// 4xx: Client Error
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},

	// 5xx: Server Error
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}
};

struct http_request {
	std::string 						method;			// GET, POST, DELETE, etc.
	std::string 						path;			// /api/users
	std::string 						http_version;	// HTTP/1.1
	std::map<std::string, std::string>	headers;		// Host, Content-Type, etc.
	std::string 						body;			// Request body content
};

const LocationConfig	*routeMatching(const std::string &uri, const std::vector<LocationConfig> &locations);
bool	checkMethod(const std::string &method, const LocationConfig *location);
std::string buildRealPath(const LocationConfig *loc, const std::string &request_path);
int	checkFile(std::string file_path, std::string method);
std::string response(const http_request &request, const std::vector<LocationConfig> &locations);

std::string	errorResponse(const int error_code);
