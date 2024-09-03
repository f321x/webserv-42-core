#include "webserver.hpp"

int main(int argc, char **argv)
{
	// check arg count ./webserv configfile_path
	if (argc != 2)
	{
		return (1);
	}

	// try to parse config file
	try
	{
		WebServerConfig config(std::string(argv[1]));
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	// init server
	WebServer server(config);
	server.serve();

	return 0;
}
