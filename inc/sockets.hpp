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

void setup_sockets(void);