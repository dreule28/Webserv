#include "socket/sockets.hpp"
#include "socket/Connection_class.hpp"

void print_poll_fds(std::vector<Connection> &poll_fds)
{
    std::cout << YELLOW << "Poll list size: " << poll_fds.size() << RESET << std::endl;
    for (size_t i = 0; i < poll_fds.size(); i++){
        std::cout << YELLOW << "FD[" << i << "]: " << poll_fds[i]._poll_fd.fd << RESET << std::endl;
	  std::cout << MAGENTA << "PORT: " << poll_fds[i]._serverConfig.port << RESET << std::endl;

	}
}

Connection connect_client(Connection &con, size_t index)
{
    Connection client_socket;
    
    client_socket = create_client_socket(con);
    client_socket._index = index;
    return(client_socket);
}

bool recv_error(ssize_t bytes)
{
    if(bytes == 0)
	{
		std::cout << YELLOW << "Client disconnected" << RESET << std::endl;
		return (true);
	}
	else if (bytes == -1)
	{
		std::cerr << RED << "recv error" << RESET << std::endl;
		return (true);
	}
    return (false);
}

void close_connection(Connection &con)
{
	if(con._poll_fd.fd != -1)
		close(con._poll_fd.fd);
	con._poll_fd.fd = -1;
	con._poll_fd.events = 0;
	con._poll_fd.revents = 0;
	con._read_buffer.clear();
	con._write_buffer.clear();
	con._write_index = 0;
}

bool is_closed_connection(Connection &c)
{
	if (c._poll_fd.fd == -1){
		return (true);
	}
	return (false);
}

void remove_closed_connection(std::vector<Connection> &con)
{
	con.erase(std::remove_if(con.begin(), con.end(),is_closed_connection), con.end());
}
