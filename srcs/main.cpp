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

void printUsage()
{
	std::cout << "Usage: ./webserv [configfile_path] [--log-level <level>]" << std::endl;
	std::cout << "Log levels: TRACE, DEBUG, INFO, WARN, ERROR" << std::endl;
}

int parseArguments(const int argc, char **argv, std::string &config_path)
{
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "--log-level")
		{
			if (i + 1 >= argc)
			{
				ERROR("Missing log level value");
				printUsage();
				return EXIT_FAILURE;
			}
			try
			{
				setLogLevel(parseLogLevel(argv[++i]));
			}
			catch (const std::exception &e)
			{
				ERROR(e.what());
				printUsage();
				return EXIT_FAILURE;
			}
		}
		else if (arg[0] != '-')
		{
			config_path = arg;
		}
		else
		{
			ERROR("Unknown argument: " + arg);
			printUsage();
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int main(const int argc, char **argv)
{
	std::string config_path = "config/test.conf";

	if (parseArguments(argc, argv, config_path) != EXIT_SUCCESS)
		return EXIT_FAILURE;

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
		return EXIT_FAILURE;
	}

	// start server
	server->serve();

	return EXIT_SUCCESS;
}
