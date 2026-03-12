#include "sockets.hpp"

void setup_sockets(void)
{
    int socket_fd;
    struct addrinfo hints;

    memset(&hints, 0 , sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    std::string port("8080");

    struct addrinfo *results;
    status = getaddrinfo(NULL, port.c_str(), &hints, &results);
    if(status != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return ;
    }

    socket_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(socket_fd == -1)
    {
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        freeaddrinfo(results);
        return ;
    }

    freeaddrinfo(results);
}