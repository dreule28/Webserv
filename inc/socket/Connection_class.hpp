#pragma once

#include "sockets.hpp"
#include <poll.h>

typedef enum FD_CHECK
{
    SERVER_FD,
    CLIENT_FD
}		    t_FD_CHECK;

class Connection{
    public:
        FD_CHECK _fd_flag;
		size_t	_index;
        
        struct pollfd _poll_fd;

        std::string _read_buffer;
        
        size_t _write_index;
        std::string _write_buffer;
        

        Connection();
        ~Connection();
        Connection(struct pollfd pollfd, FD_CHECK is_server, int index, size_t write_index);

};
