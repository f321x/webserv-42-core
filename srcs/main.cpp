#include "WebServer.hpp"

volatile std::sig_atomic_t shutdown_signal = false;
WebServer *server = nullptr;

void shutdownApplication(int signal)
{
	if (signal == SIGINT)
	{
		INFO("Shutting down application");
		shutdown_signal = true;
		exit(0);
	}
}

void cleanupExit()
{
	if (server)
	{
		delete server;
	}
}

int main(int argc, char **argv)
{
	// check arg count ./webserv configfile_path
	if (argc != 2)
	{
		ERROR("Usage: ./webserv configfile_path");
		return (1);
	}

	try
	{
		// try to parse config file
		WebServerConfig config = WebServerConfig(std::string(argv[1]));
		// init WebServer
		server = new WebServer(config, &shutdown_signal);
	}
	catch (std::exception &e)
	{
		ERROR(e.what());
		return (1);
	}
	atexit(cleanupExit);
	signal(SIGINT, shutdownApplication);

	// start server
	server->serve();
	delete server;

	return 0;
}
