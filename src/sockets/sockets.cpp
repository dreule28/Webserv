#include "sockets.hpp"

void print_poll_fds(std::vector<Socket> &poll_fds)
{
    std::cout << YELLOW << "Poll list size: " << poll_fds.size() << RESET << std::endl;
    for (size_t i = 0; i < poll_fds.size(); i++){
        std::cout << YELLOW << "FD[" << i << "]: " << poll_fds[i]._poll_fd.fd << RESET << std::endl;
    }
}

void event_loop(std::vector<Socket> fds){
	
	while (true){
		bool break_flag = false;

		std::vector<struct pollfd> poll_array(fds.size());
        
        for (size_t i = 0; i < fds.size(); i++){
            poll_array[i] = fds[i]._poll_fd;
        }
        
        int ready = poll(poll_array.data(), fds.size(), -1);
        if (ready == -1){
            std::cerr << RED << "poll error" << RESET << std::endl;
            break;
        }
        for(size_t i = 0; i < fds.size(); i++){
            if(poll_array[i].revents & POLLIN){
                std::cout << BLUE << "POLLIN" << RESET << std::endl;
                break_flag = true;
            }
        }
		if(break_flag)
			break;
	}

}

void setup_sockets(void)
{
    std::string port("8080");
    
    std::vector<Socket> poll_fds;
    int i = 0;
    while(i < 1){
        int socket_fd = create_socket_fds(port);
        if(socket_fd == -1){
			return ;
        }
		Socket fd;
		fd._poll_fd.fd = socket_fd;
        fd._poll_fd.events = POLLIN;
        fd._poll_fd.revents = 0;
        fd._index = i;

        poll_fds.push_back(fd);
        i++;
    }
	print_poll_fds(poll_fds);
	event_loop(poll_fds);

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
    std::cout << GREEN << "Socket created " << socket_fd << RESET << std::endl;

     int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << RED << "setsockopt error: " << strerror(errno) << RESET << std::endl;
        freeaddrinfo(results);
        return (-1);
    }
    std::cout << GREEN << "Socket options set"  << RESET << std::endl;


    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << RED << "fcntl error: " << strerror(errno) << RESET << std::endl;
        freeaddrinfo(results);
        return (-1);
    }
    std::cout << GREEN << "Socket set to non-blocking" << RESET << std::endl;

    if (bind(socket_fd, results->ai_addr, results->ai_addrlen) == -1)
    {
        std::cerr << RED << "bind error: " << strerror(errno) << RESET << std::endl;
        close(socket_fd);
        freeaddrinfo(results);
        return (-1);
    }
    std::cout << GREEN << "Socket bound to port " << port << RESET << std::endl;
    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        std::cerr << RED << "listen error: " << strerror(errno) << RESET << std::endl;
        close(socket_fd);
        freeaddrinfo(results);
        return (-1);
    }
    std::cout << GREEN << "Socket listening on port " << port << RESET << std::endl;
    freeaddrinfo(results);

    return(socket_fd);
}
