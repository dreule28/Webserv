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

// sockets.cpp
void setup_sockets(void);
int create_socket_fds(std::string port);