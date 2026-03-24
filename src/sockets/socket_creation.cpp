#include "socket/sockets.hpp"
#include "socket/Connection_class.hpp"

 std::vector<Connection> setup_sockets(void)
{
    std::string port("8080");
    
    std::vector<Connection> con;
    int i = 0;
    while(i < 1){
        int socket_fd = create_socket_fds(port);
        if(socket_fd == -1)
        {
            std::cout << RED << "socket_fd creation failed" << std::endl;
			break;
        }
        
		Connection fd;
		fd._poll_fd.fd = socket_fd;
        fd._poll_fd.events = POLLIN;
        fd._poll_fd.revents = 0;
        fd._index = i;

        con.push_back(fd);
        i++;
    }
	print_poll_fds(con);
    return(con);
}

int create_socket_fds(std::string port)
{
    int socket_fd;
    struct addrinfo hints;

    memset(&hints, 0 , sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;

    struct addrinfo *results;
    status = getaddrinfo(NULL, port.c_str(), &hints, &results);
    if(status != 0)
    {
        std::cerr << RED << "getaddrinfo error: " << gai_strerror(status) << RESET << std::endl;
        return (-1);
    }

    socket_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(socket_fd == -1)
    {
        std::cerr << RED << "socket error: " << strerror(errno) << RESET << std::endl;
        freeaddrinfo(results);
        return (-1);
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << RED << "setsockopt error: " << strerror(errno) << RESET << std::endl;
        freeaddrinfo(results);
        return (-1);
    }


    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << RED << "fcntl error: " << strerror(errno) << RESET << std::endl;
        freeaddrinfo(results);
        return (-1);
    }

    if (bind(socket_fd, results->ai_addr, results->ai_addrlen) == -1)
    {
        std::cerr << RED << "bind error: " << strerror(errno) << RESET << std::endl;
        close(socket_fd);
        freeaddrinfo(results);
        return (-1);
    }
    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        std::cerr << RED << "listen error: " << strerror(errno) << RESET << std::endl;
        close(socket_fd);
        freeaddrinfo(results);
        return (-1);
    }
    
    freeaddrinfo(results);

    return(socket_fd);
}
