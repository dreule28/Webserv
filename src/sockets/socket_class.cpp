#include "socket/Connection_class.hpp"

Connection::Connection()
    : _fd_flag(SERVER_FD), _index(0)
{
    _poll_fd.fd = -1;
    _poll_fd.events = 0;
    _poll_fd.revents = 0;
};


Connection::~Connection()
{};

Connection::Connection(struct pollfd pollfd, FD_CHECK fd, int index)
    : _fd_flag(fd), _index(index), _poll_fd(pollfd)
{};