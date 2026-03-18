#include "socket/sockets.hpp"
#include "socket/Connection_class.hpp"

void print_poll_fds(std::vector<Connection> &poll_fds)
{
    std::cout << YELLOW << "Poll list size: " << poll_fds.size() << RESET << std::endl;
    for (size_t i = 0; i < poll_fds.size(); i++){
        std::cout << YELLOW << "FD[" << i << "]: " << poll_fds[i]._poll_fd.fd << RESET << std::endl;
    }
}

Connection create_client_socket(Connection fds)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    int client_fd = accept(fds._poll_fd.fd, 
                 (struct sockaddr *)&client_addr, 
                  &client_addr_len);
    if(client_fd == -1){
        std::cerr << RED << "accept error" << RESET << std::endl;
    } else {
        std::cout << GREEN << "Client connected: " << client_fd << RESET << std::endl;
    }
    
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << RED << "fcntl error: " << strerror(errno) << RESET << std::endl;
        exit(-1);
    }
// Create new Socket for the client
    Connection client_socket;
    client_socket._poll_fd.fd = client_fd;
    client_socket._poll_fd.events = POLLIN;
    client_socket._poll_fd.revents = 0;
    client_socket._fd_flag = CLIENT_FD;
    
    return(client_socket);
}

void handleRequest(Connection fd)
{
    char buffer[200];
    ssize_t bytes = recv(fd._poll_fd.fd, buffer, sizeof(buffer), 0);
    if(bytes == 0){
        std::cout << YELLOW << "Client disconnected" << RESET << std::endl;
        return ;
    } else if (bytes == -1){
        std::cerr << RED << "recv error: " << strerror(errno) << RESET << std::endl;
        return ;
    }

    buffer[bytes] = '\0';
    fd._read_buffer.append(buffer, bytes);
    std::cout << BOLD << BLUE << fd._read_buffer << RESET << std::endl;
}

void event_loop(std::vector<Connection> fds){
	
	while (true){

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
                if(fds[i]._fd_flag == SERVER_FD){
                    std::cout << BLUE << "SERVER_FD route" << RESET << std::endl;
                        Connection client_socket;

                        client_socket = create_client_socket(fds[i]);
                        client_socket._index = fds.size();
                        fds.push_back(client_socket);
                    } else if(fds[i]._fd_flag == CLIENT_FD){
                    std::cout << BLUE << "CLIENT_FD route" << RESET << std::endl;
                    handleRequest(fds[i]);
                }
        
            }
        }
        }

}



void setup_sockets(void)
{
    std::string port("8080");
    
    std::vector<Connection> poll_fds;
    int i = 0;
    while(i < 1){
        int socket_fd = create_socket_fds(port);
        if(socket_fd == -1){
			return ;
        }
		Connection fd;
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
