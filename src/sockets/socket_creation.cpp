#include "socket/sockets.hpp"
#include "socket/Connection_class.hpp"

std::vector<Connection> setup_sockets(Config config)
{
    std::vector<Connection> con;
    std::map<std::string, size_t> listenerIndex;

    for (size_t i = 0; i < config.servers.size(); i++) {
        const ServerConfig& srv = config.servers[i];
        std::string key = srv.host + ":" + std::to_string(srv.port);

        if (listenerIndex.count(key)) {
            con[listenerIndex[key]]._vhosts.push_back(srv);
            continue;
        }

        int socket_fd = create_socket_fds(std::to_string(srv.port));
        if (socket_fd == -1) {
            std::cout << RED << "socket_fd creation failed" << std::endl;
            continue;
        }

        Connection fd;
        fd._poll_fd.fd = socket_fd;
        fd._poll_fd.events = POLLIN;
        fd._poll_fd.revents = 0;
        fd._index = i;
        fd._serverConfig = srv;
        fd._vhosts.push_back(srv);

        con.push_back(fd);
        listenerIndex[key] = con.size() - 1;
    }
    print_poll_fds(con);
    return con;
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
        throw std::runtime_error("getaddrinfo error");
    }

    socket_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(socket_fd == -1)
    {
        freeaddrinfo(results);
        throw std::runtime_error("socket error");
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        freeaddrinfo(results);
	  close(socket_fd);
        throw std::runtime_error("setsockopt error");
    }


    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        freeaddrinfo(results);
        throw std::runtime_error("fcntl error");
    }

    if (bind(socket_fd, results->ai_addr, results->ai_addrlen) == -1)
    {
        close(socket_fd);
        freeaddrinfo(results);
        throw std::runtime_error("bind error");
    }
    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        close(socket_fd);
        freeaddrinfo(results);
        throw std::runtime_error("listen error");
    }
    
    freeaddrinfo(results);

    return(socket_fd);
}
