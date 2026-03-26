#include "webserv.hpp"

void webServ(char *filename) 
{
	Config config;
    ConfigParser parser;

    config = parser.parse(filename);
    // printConfig(config);
    std::vector<Connection> con;
    con = setup_sockets(config);
    event_loop(con);
}
