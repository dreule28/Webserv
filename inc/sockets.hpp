#pragma once

#include "webserv.hpp"

#include <sys/types.h>
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>     
#include <fcntl.h>         
#include <unistd.h>        
#include <cstring>         
#include <cerrno>
#include <netdb.h>
#include <vector>
#include <poll.h>

typedef enum FD_CHECK // got the redirecting types for the files
{
    SERVER_FD,
    CLIENT_FD
}		    t_FD_CHECK;

class Socket{
    public:
        FD_CHECK _fd_flag;
		size_t	_index;
        struct pollfd _poll_fd;

        Socket();
        ~Socket();
        Socket(struct pollfd pollfd, FD_CHECK is_server, int index);

};

// sockets.cpp
void setup_sockets(void);
int create_socket_fds(std::string port);