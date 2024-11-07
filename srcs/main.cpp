#include "WebServer.hpp"

std::unique_ptr<WebServer> server = nullptr;

void shutdownApplication(int signal)
{
	if (signal == SIGINT)
	{
		INFO("Shutting down application");
		exit(0);
	}
}

int main(const int argc, char **argv)
{
	std::string config_path;
	// check arg count ./webserv configfile_path
	if (argc > 2)
	{
		ERROR("Usage: ./webserv configfile_path");
		return (1);
	}
	else if (argc == 1)
	{
		WARN("No config file provided, using default config");
		config_path = "config/test.conf";
	}
	else
		config_path = argv[1];
	signal(SIGINT, shutdownApplication);

	try
	{
		// try to parse config file
		WebServerConfig config = WebServerConfig(config_path);
		if (LOG_LEVEL <= DEBUG)
			config.printConfig();
		// init WebServer
		server = std::make_unique<WebServer>(config);
	}
	catch (std::exception &e)
	{
		ERROR(e.what());
		return (1);
	}

	// start server
	server->serve();

	return 0;
}
