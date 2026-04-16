#include "socket/Connection_class.hpp"

void build_chunked_body(Connection &con)
{
	size_t delimiter_pos = con._read_buffer.find("\r\n\r\n");
	if (delimiter_pos == std::string::npos)
		return;

	std::string body = con._read_buffer.substr(delimiter_pos + 4);
	if (body.find("0\r\n\r\n") == std::string::npos)
		return;

	con._fullReq.parseChunked(con._fullReq, body);
}