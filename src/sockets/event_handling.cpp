#include "socket/Connection_class.hpp"
#include "socket/sockets.hpp"
#include "HttpResponse.hpp"

Connection create_client_socket(Connection con)
{
	Connection client_socket;
	client_socket._poll_fd.fd = -1;
	client_socket._poll_fd.events = 0;
	client_socket._poll_fd.revents = 0;
	client_socket._fd_flag = CLIENT_FD;
	client_socket._write_index = 0;
	client_socket._serverConfig = con._serverConfig;

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	int client_fd = accept(con._poll_fd.fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if(client_fd == -1)
	{
		std::cerr << RED << "accept error" << RESET << std::endl;
		return (client_socket);
    }
	else
	{
		std::cout << GREEN << "Client connected: " << client_fd << RESET << std::endl;
    	}

    	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    	{
			std::cerr << RED << "fcntl error: " << strerror(errno) << RESET << std::endl;
			close(client_fd);
      		return (client_socket);
    	}

	client_socket._poll_fd.fd = client_fd;
	client_socket._poll_fd.events = POLLIN;

    return(client_socket);
}

void handle_pollin_request(Connection &con)
{
	bool isChuncked = true;
	char buffer[RECV_BUFFER_SIZE];
	ssize_t bytes = recv(con._poll_fd.fd, buffer, sizeof(buffer), 0);
	if(recv_error(bytes))
	{
		close_connection(con);
		return ;
	}

	con._read_buffer.append(buffer, static_cast<size_t>(bytes));
	size_t delimiter_pos = con._read_buffer.find("\r\n\r\n");
	if(delimiter_pos != std::string::npos)
	{
		std::string headers = con._read_buffer.substr(0, delimiter_pos + 4);

		con._fullReq = con._fullReq.parseRequest(headers);
		if(con._fullReq._method == POST && isChuncked == true)
		{
			build_chunked_body(con);
		}
		if(con._fullReq._method == POST)
		{
			std::string body = con._read_buffer.substr(delimiter_pos + 4);
			if(con._fullReq._contentLength == sizeof(body))
			{
				con._write_buffer = response(con._fullReq, con._serverConfig.locations);
				con._poll_fd.events = POLLOUT;
				return;
			}
			con._fullReq._body = body;
		}
		
		con._write_buffer = response(con._fullReq, con._serverConfig.locations);
		con._poll_fd.events = POLLOUT;
	}
}

void handle_pollout_request(Connection &con)
{
    	if(con._write_buffer.empty())
    	{
		std::cout << GREEN << "write buffer empty write done" << RESET << std::endl;
		close_connection(con);
		return ;
    	}

    	const char *buffer = con._write_buffer.c_str() + con._write_index;
    	size_t remaining = con._write_buffer.length() - con._write_index;

    	ssize_t bytes = send(con._poll_fd.fd, buffer, remaining, 0);
    	if(bytes == 0)
    	{
		std::cout << YELLOW << "Client disconnected" << RESET << std::endl;
		close_connection(con);
		return ;
    	}
    	else if (bytes == -1)
	{
		std::cerr << RED << "send error" << RESET << std::endl;
		close_connection(con);
		return ;
    	}
    	con._write_index += bytes;
}

void handle_pollerr_pollhup_request(Connection &con)
{
	std::cout << RED << "POLLERR/POLLHUP route" << RESET << std::endl;
	close_connection(con);
}