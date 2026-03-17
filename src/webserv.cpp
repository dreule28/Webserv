#include "webserv.hpp"

namespace {
	std::string joinMethods(const std::vector<std::string>& methods)
	{
		std::string joined;

		for (std::size_t i = 0; i < methods.size(); ++i)
		{
			if (i > 0)
				joined += ", ";
			joined += methods[i];
		}
		return joined;
	}

	void printLocation(const LocationConfig& location, std::size_t index)
	{
		std::cout << "  location[" << index << "]" << std::endl;
		if (!location.path.empty())
			std::cout << "    path: " << location.path << std::endl;
		if (!location.root.empty())
			std::cout << "    root: " << location.root << std::endl;
		if (!location.methods.empty())
			std::cout << "    methods: " << joinMethods(location.methods) << std::endl;
		if (!location.uploadDir.empty())
			std::cout << "    upload_dir: " << location.uploadDir << std::endl;
		if (location.cgiEnabled)
			std::cout << "    cgi_enabled: true" << std::endl;
		if (!location.cgiExtensions.empty())
			std::cout << "    cgi_extension: " << location.cgiExtensions << std::endl;
		if (!location.cgiPath.empty())
			std::cout << "    cgi_path: " << location.cgiPath << std::endl;
	}

	void printConfig(const Config& config)
	{
		std::cout << "Parsed config" << std::endl;
		for (std::size_t i = 0; i < config.servers.size(); ++i)
		{
			const ServerConfig& server = config.servers[i];

			std::cout << "server[" << i << "]" << std::endl;
			if (server.port > 0)
				std::cout << "  listen: " << server.port << std::endl;
			if (!server.host.empty())
				std::cout << "  host: " << server.host << std::endl;
			if (!server.root.empty())
				std::cout << "  root: " << server.root << std::endl;
			if (!server.index.empty())
				std::cout << "  index: " << server.index << std::endl;
			for (std::size_t j = 0; j < server.locations.size(); ++j)
				printLocation(server.locations[j], j);
		}
	}
}

void webServ(char *filename)
{
    Config config;
    ConfigParser parser;

    config = parser.parse(filename);
    printConfig(config);
	
    // setup_sockets();

    // //anakin muss bissle warten auf uns
    // anakin_part();
}
