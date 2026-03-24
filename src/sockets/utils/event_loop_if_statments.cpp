#include "socket/sockets.hpp"

bool isServerFd(Connection con)
{
    if(con._fd_flag == SERVER_FD){
        return (true);
    }else{
        return (false);
    }
}

bool isClientFd(Connection con)
{
    if(con._fd_flag == CLIENT_FD){
        return (true);
    }else{
        return (false);
    }
}

bool isPOLLIN(struct pollfd poll_array)
{
    if(poll_array.revents & POLLIN){
        return (true);
    } else {
        return (false);
    }
}

bool isPOLLOUT(struct pollfd poll_array)
{
    if(poll_array.revents & POLLOUT){
        return (true);
    } else {
        return (false);
    }
}

bool isPOLLERR(struct pollfd poll_array)
{
    if(poll_array.revents & POLLERR){
        return (true);
    } else {
        return (false);
    }
}

bool isPOLLHUP(struct pollfd poll_array)
{
    if(poll_array.revents & POLLHUP){
        return (true);
    } else {
        return (false);
    }
}
