#include "WebServer.hpp"

int main(int argc, char **argv)
{
	// check arg count ./webserv configfile_path
	if (argc != 2)
	{
		ERROR("Usage: ./webserv configfile_path");
		return (1);
	}

	WebServer *server;
	try
	{
		// try to parse config file
		WebServerConfig config = WebServerConfig(std::string(argv[1]));
		// init WebServer
		server = new WebServer(config);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	// start server
	server->serve();

	return 0;
}
