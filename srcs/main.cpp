#include "WebServer.hpp"

int main(int argc, char **argv)
{
	// check arg count ./webserv configfile_path
	if (argc != 2)
	{
		return (1);
	}

	try
	{
		// try to parse config file
		WebServerConfig config(std::string(argv[1]));

		// init WebServer
		WebServer server(config);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return 0;
}
