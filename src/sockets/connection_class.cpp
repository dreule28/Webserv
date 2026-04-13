#include "socket/Connection_class.hpp"

Connection::Connection()
    : _fd_flag(SERVER_FD), _index(0), _write_index(0),
      _cgi_state(NO_CGI), _cgi_pid(-1), _cgi_stdout_fd(-1),
      _cgi_stdin_fd(-1), _cgi_stdin_written(0), _cgi_start_time(0)
{
    _poll_fd.fd = -1;
    _poll_fd.events = 0;
    _poll_fd.revents = 0;
};


Connection::~Connection()
{};

Connection::Connection(struct pollfd pollfd, FD_CHECK fd, int index, size_t write_index)
    : _fd_flag(fd), _index(index), _poll_fd(pollfd), _write_index(write_index),
      _cgi_state(NO_CGI), _cgi_pid(-1), _cgi_stdout_fd(-1),
      _cgi_stdin_fd(-1), _cgi_stdin_written(0), _cgi_start_time(0)
{};
