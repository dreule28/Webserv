#include "socket/sockets.hpp"

bool isServerFd(Connection con)
{
    if(con._fd_flag == SERVER_FD){
        return (true);
    }else{
        return (false);
    }
}

bool isServerFd(Connection con)
{
    if(con._fd_flag == CLIENT_FD){
        return (true);
    }else{
        return (false);
    }
}

