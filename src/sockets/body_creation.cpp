#include "socket/Connection_class.hpp"

void build_chunked_body(Connection &con)
{
    size_t delimiter_pos = con._read_buffer.find("0\r\n\r\n");
    std::string body = con._read_buffer.substr(delimiter_pos + 4);
    
    if(delimiter_pos != std::string::npos)
    {
        return;
    }
    con._fullReq._body = body;
}