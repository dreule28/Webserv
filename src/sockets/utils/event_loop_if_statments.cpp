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

bool isPOLLIN(Connection con)
{
    if(con._poll_fd.revents == POLLIN){
        return (true);
    } else {
        return (false);
    }
}

bool isPOLLOUT(Connection con)
{
    if(con._poll_fd.revents == POLLOUT){
        return (true);
    } else {
        return (false);
    }
}

bool isPOLLERR(Connection con)
{
    if(con._poll_fd.revents == POLLERR){
        return (true);
    } else {
        return (false);
    }
}