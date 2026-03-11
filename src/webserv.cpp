#include "webserv.hpp"

void webServ(char *filename)
{
    parse_config(filename);
    setup_sockets();

    //anakin muss bissle warten auf uns
    anakin_part();
}