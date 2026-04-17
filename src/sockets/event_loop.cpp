#include "socket/sockets.hpp"
#include  "socket/Connection_class.hpp"

void event_loop(std::vector<Connection> &con)
{

	while (true)
	{
		std::vector<struct pollfd> poll_array;
		std::vector<size_t> poll_to_con_index;
		std::vector<int> poll_type;

		for (size_t i = 0; i < con.size(); i++)
		{
			poll_array.push_back(con[i]._poll_fd);
			poll_to_con_index.push_back(i);
			poll_type.push_back(0);

			if (con[i]._cgi_state == CGI_RUNNING)
			{
				if (con[i]._cgi_stdout_fd != -1)
				{
					struct pollfd cgi_stdout_poll;
					cgi_stdout_poll.fd = con[i]._cgi_stdout_fd;
					cgi_stdout_poll.events = POLLIN;
					cgi_stdout_poll.revents = 0;
					poll_array.push_back(cgi_stdout_poll);
					poll_to_con_index.push_back(i);
					poll_type.push_back(1);
				}

				if (con[i]._cgi_stdin_fd != -1 && con[i]._cgi_stdin_written < con[i]._fullReq._body.size())
				{
					struct pollfd cgi_stdin_poll;
					cgi_stdin_poll.fd = con[i]._cgi_stdin_fd;
					cgi_stdin_poll.events = POLLOUT;
					cgi_stdin_poll.revents = 0;
					poll_array.push_back(cgi_stdin_poll);
					poll_to_con_index.push_back(i);
					poll_type.push_back(2);
				}

				// Check for CGI timeout
				check_cgi_timeout(con[i]);
			}
		}

		int ready = poll(poll_array.data(), poll_array.size(), 1000);
		if (ready == -1)
		{
			std::cerr << RED << "poll error" << RESET << std::endl;
			break;
		}

		std::vector<Connection> new_clients;

		for(size_t i = 0; i < poll_array.size(); i++)
		{
			size_t con_idx = poll_to_con_index[i];
			int type = poll_type[i];

			if (type == 0)
			{
				if(isPOLLIN(poll_array[i]))
				{
					std::cout << BLUE << "POLLIN" << RESET << std::endl;
					if(isServerFd(con[con_idx]))
					{
						std::cout << BLUE << "SERVER_FD route" << RESET << std::endl;
						Connection client = connect_client(con[con_idx], con.size() + new_clients.size());
						if(client._poll_fd.fd != -1)
							new_clients.push_back(client);
					}
					if(isClientFd(con[con_idx]))
					{
						std::cout << BLUE << "CLIENT_FD route" << RESET << std::endl;
						handle_pollin_request(con[con_idx]);
					}
				}
				if(isPOLLOUT(poll_array[i]))
				{
					std::cout << BLUE << "POLLOUT route" << RESET << std::endl;
					handle_pollout_request(con[con_idx]);
				}
				if(isPOLLERR(poll_array[i]) || isPOLLHUP(poll_array[i]))
				{
					std::cout << BLUE << "POLLERR/POLLHUP" << RESET << std::endl;
					handle_pollerr_pollhup_request(con[con_idx]);
				}
			}
			else if (type == 1)
			{
				if (isPOLLIN(poll_array[i]))
				{
					std::cout << BLUE << "CGI STDOUT POLLIN" << RESET << std::endl;
					handle_cgi_stdout(con[con_idx]);
				}
				if (isPOLLERR(poll_array[i]) || isPOLLHUP(poll_array[i]))
				{
					std::cout << RED << "CGI STDOUT ERROR/HUP" << RESET << std::endl;
					handle_cgi_stdout(con[con_idx]);
				}
			}
			else if (type == 2)
			{
				if (isPOLLOUT(poll_array[i]))
				{
					std::cout << BLUE << "CGI STDIN POLLOUT" << RESET << std::endl;
					handle_cgi_stdin(con[con_idx]);
				}
				if (isPOLLERR(poll_array[i]) || isPOLLHUP(poll_array[i]))
				{
					std::cout << RED << "CGI STDIN ERROR/HUP" << RESET << std::endl;
					// Close stdin, can't write anymore
					if (con[con_idx]._cgi_stdin_fd != -1)
					{
						close(con[con_idx]._cgi_stdin_fd);
						con[con_idx]._cgi_stdin_fd = -1;
					}
				}
			}
		}
		for (auto& client : new_clients)
			con.push_back(client);

		remove_closed_connection(con);
	}
}
