#pragma once

#include "webserv.hpp"
#include "Connection_class.hpp"

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

class Connection;

// event_handling.cpp
Connection create_client_socket(Connection con);
void handle_pollin_request(Connection &con);
void handle_pollout_request(Connection &con);

// event_loop.cpp
void event_loop(std::vector<Connection> &con);

// socket_creation.cpp
 std::vector<Connection> setup_sockets(void);
int create_socket_fds(std::string port);

// utils.cpp
void print_poll_fds(std::vector<Connection> &poll_fds);
Connection connect_client(Connection &con, size_t index);
bool recv_error(ssize_t bytes);

// event_loop_if_statments.cpp
bool isServerFd(Connection con);
bool isClientFd(Connection con);
bool isPOLLIN(struct pollfd poll_array);
bool isPOLLOUT(struct pollfd poll_array);
bool isPOLLERR(struct pollfd poll_array);