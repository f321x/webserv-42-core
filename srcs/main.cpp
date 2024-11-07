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

// int main(int argc, char **argv)
// {
// 	std::string config_path;
// 	// check arg count ./webserv configfile_path
// 	if (argc > 2)
// 	{
// 		ERROR("Usage: ./webserv configfile_path");
// 		return (1);
// 	}
// 	else if (argc == 1)
// 	{
// 		WARN("No config file provided, using default config");
// 		config_path = "config/test.conf";
// 	}
// 	else
// 		config_path = argv[1];
// 	signal(SIGINT, shutdownApplication);

// 	try
// 	{
// 		// try to parse config file
// 		WebServerConfig config = WebServerConfig(config_path);
// 		if (LOG_LEVEL <= DEBUG)
// 			config.printConfig();
// 		// init WebServer
// 		server = std::make_unique<WebServer>(config);
// 	}
// 	catch (std::exception &e)
// 	{
// 		ERROR(e.what());
// 		return (1);
// 	}

// 	// start server
// 	server->serve();

// 	return 0;
// }

#include <iostream>
#include "RequestPacket.hpp"

int main()
{
	RequestPacket packet;

	// Test case 1: Valid chunked data
	try
	{
		std::string chunked_data = "4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n";
		bool result = packet.appendChunkedData(chunked_data);
		std::cout << packet.getContent() << std::endl;
		std::cout << "Test case 1 (valid chunked data): " << (result ? "Passed" : "Failed") << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Test case 1 (valid chunked data): Failed with exception: " << e.what() << std::endl;
	}

	// Test case 2: Invalid chunked data (invalid chunk size)
	try
	{
		std::string chunked_data = "G\r\nInvalid\r\n0\r\n\r\n";
		bool result = packet.appendChunkedData(chunked_data);
		std::cout << "Test case 2 (invalid chunk size): " << (result ? "Passed" : "Failed") << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Test case 2 (invalid chunk size): Passed with exception: " << e.what() << std::endl;
	}

	// Test case 3: Incomplete chunked data
	try
	{
		std::string chunked_data = "4\r\nWiki";
		bool result = packet.appendChunkedData(chunked_data);
		std::cout << "Test case 3 (incomplete chunked data): " << (result ? "Passed" : "Failed") << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Test case 3 (incomplete chunked data): Failed with exception: " << e.what() << std::endl;
	}

	// Test case 4: Empty chunked data
	try
	{
		std::string chunked_data = "";
		bool result = packet.appendChunkedData(chunked_data);
		std::cout << "Test case 4 (empty chunked data): " << (result ? "Passed" : "Failed") << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Test case 4 (empty chunked data): Failed with exception: " << e.what() << std::endl;
	}

	return 0;
}
