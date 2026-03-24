#include "webserv.hpp"

void webServ(char *filename)
{
    parse_config(filename);
    std::vector<Connection> con;
    con = setup_sockets();
    event_loop(con);

}