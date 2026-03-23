#include "socket/sockets.hpp"
#include  "socket/Connection_class.hpp"

void event_loop(std::vector<Connection> &con){
	
	while (true){

	std::vector<struct pollfd> poll_array(con.size());
        for (size_t i = 0; i < con.size(); i++){
            poll_array[i] = con[i]._poll_fd;
        }
        
        int ready = poll(poll_array.data(), con.size(), -1);
        if (ready == -1){
            std::cerr << RED << "poll error" << RESET << std::endl;
            break;
        }

        //check revnets for each connection
        for(size_t i = 0; i < con.size(); i++)
        {
	    if(isPOLLIN(poll_array[i]))
            {
        	std::cout << BLUE << "POLLIN" << RESET << std::endl;
                if(isServerFd(con[i]))
                {
                    std::cout << BLUE << "SERVER_FD route" << RESET << std::endl;
                    Connection client_socket;

                    client_socket = create_client_socket(con[i]);
                    client_socket._index = con.size();
                    con.push_back(client_socket);
                }
                else if(isClientFd(con[i]))
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
        }
    }

}