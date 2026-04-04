#include "webserv.hpp"

// void printConfig(const Config& config) {
// 	for (const ServerConfig& s : config.servers) {
// 		std::cout << "server " << s.host << ":" << s.port
// 		          << " root=" << s.root << " index=" << s.index << "\n";
// 		for (const LocationConfig& loc : s.locations) {
// 			std::cout << "  location " << loc.path;
// 			for (const std::string& m : loc.methods)
// 				std::cout << " " << m;
// 			if (!loc.root.empty())      std::cout << " root=" << loc.root;
// 			if (!loc.uploadDir.empty()) std::cout << " upload=" << loc.uploadDir;
// 			if (loc.cgiEnabled)         std::cout << " cgi=" << loc.cgiExtensions << ":" << loc.cgiPath;
// 			std::cout << "\n";
// 		}
// 	}
// }

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
