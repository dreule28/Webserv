#pragma once

#include "sockets.hpp"
#include <poll.h>
#include "../Config/HttpRequest.hpp"
#include "../Config/Config.hpp"


#define RECV_BUFFER_SIZE 4096
#define CGI_TIMEOUT 30

typedef enum FD_CHECK
{
    SERVER_FD,
    CLIENT_FD
}		    t_FD_CHECK;

typedef enum CgiState
{
    NO_CGI,
    CGI_RUNNING,
    CGI_DONE,
    CGI_ERROR
}           t_CgiState;

class Connection{
    public:
        FD_CHECK _fd_flag;
		size_t	_index;

        struct pollfd _poll_fd;

        std::string _read_buffer;
		HttpRequest _fullReq;
		ServerConfig _serverConfig;
        size_t _write_index;
        std::string _write_buffer;

        // CGI state tracking
        CgiState _cgi_state;
        pid_t _cgi_pid;
        int _cgi_stdout_fd;      // Read end of stdout pipe
        int _cgi_stdin_fd;       // Write end of stdin pipe
        std::string _cgi_output;
        size_t _cgi_stdin_written;  // Bytes of POST body written
        time_t _cgi_start_time;
        std::string _cgi_script_path;
        std::string _cgi_path;

        Connection();
        ~Connection();
        Connection(struct pollfd pollfd, FD_CHECK is_server, int index, size_t write_index);

};
