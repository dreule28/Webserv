#include "socket/sockets.hpp"
#include  "socket/Connection_class.hpp"

void event_loop(std::vector<Connection> &con)
{

	while (true)
	{
		std::vector<struct pollfd> poll_array(con.size());
		for (size_t i = 0; i < con.size(); i++)
			poll_array[i] = con[i]._poll_fd;

		int ready = poll(poll_array.data(), con.size(), -1);
		if (ready == -1)
		{
			std::cerr << RED << "poll error" << RESET << std::endl;
			break;
		}

		std::vector<Connection> new_clients;

		//check revents for each connection
		for(size_t i = 0; i < con.size(); i++)
		{
			if(isPOLLIN(poll_array[i]))
			{
				std::cout << BLUE << "POLLIN" << RESET << std::endl;
				if(isServerFd(con[i]))
				{
					std::cout << BLUE << "SERVER_FD route" << RESET << std::endl;
					Connection client = connect_client(con[i], con.size() + new_clients.size());
					if(client._poll_fd.fd != -1)
						new_clients.push_back(client);
				}
				if(isClientFd(con[i]))
				{
					std::cout << BLUE << "CLIENT_FD route" << RESET << std::endl;
					handle_pollin_request(con[i]);
				}
			}
			if(isPOLLOUT(poll_array[i]))
			{
				std::cout << BLUE << "POLLOUT route" << RESET << std::endl;
				handle_pollout_request(con[i]);
			}
			if(isPOLLERR(poll_array[i]) || isPOLLHUP(poll_array[i]))
			{
				std::cout << BLUE << "POLLERR/POLLHUP" << RESET << std::endl;
				handle_pollerr_pollhup_request(con[i]);
			}
		}
        	for (auto& client : new_clients)
            	con.push_back(client);

		remove_closed_connection(con);
	}
}