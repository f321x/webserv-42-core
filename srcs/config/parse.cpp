#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include "logging.hpp"
#include <string>
#include <sstream>
#include <iostream>

static void ensureNoTrailingTokens(std::istringstream& stream)
{
	std::string trailing_token;
	if (stream >> trailing_token)
		throw std::runtime_error("Unexpected trailing token: " + trailing_token);
}

void parseListen(ServerConfig& server, std::istringstream& stream)
{
	std::string host_port;
	stream >> host_port;

	if (host_port.back() == ';')
		host_port.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the listen directive");

	std::istringstream host_port_stream(host_port);
	std::string host;
	int port;

	if (std::getline(host_port_stream, host, ':')) {
		if (host_port_stream >> port) {
			server.setHost(host);
			server.setPort(port);
		} else {
			try {
				port = std::stoi(host);  // Handle the case where only a port is provided
			} catch (const std::invalid_argument& e) {
				throw std::runtime_error("Invalid port: " + host);
			}
			server.setPort(port);
		}
	}
	ensureNoTrailingTokens(stream);
	INFO("ADDED Host: " + server.getHost() + " Port: " + std::to_string(server.getPort()));
}

void parseServerName(ServerConfig& server, std::istringstream& stream)
{
	std::string server_name;
	stream >> server_name;

	if (server_name.back() == ';')
		server_name.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the server_name directive");

	server.addServerName(server_name);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Server name: " + server_name);
}

void parseErrorPage(ServerConfig& server, std::istringstream& stream)
{
	int error_code;
	std::string error_page;

	if (!(stream >> error_code))
		throw std::runtime_error("Invalid error code");
	if (!(stream >> error_page))
		throw std::runtime_error("Invalid error page");

	if (error_page.back() == ';')
		error_page.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the error_page directive");

	server.addErrorPage(error_code, error_page);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Error page: " + std::to_string(error_code) + " " + error_page);
}

void parseClientMaxBodySize(ServerConfig& server, std::istringstream& stream)
{
	int size;
	if (!(stream >> size))
		throw std::runtime_error("Invalid client max body size");

	std::string leftover;
	stream >> leftover;
	if (leftover != ";" && !leftover.empty())
		throw std::runtime_error("Expected ';' at the end of the client_max_body_size directive");

	server.setClientMaxBodySize(size);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Client max body size: " + std::to_string(size));
}

void parseRoot(RouteConfig& route, std::istringstream& stream)
{
	std::string root;
	stream >> root;

	if (root.back() == ';')
		root.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the root directive");

	route.setRoot(root);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Root: " + route.getRoot());
}

void parseAcceptedMethods(RouteConfig& route, std::istringstream& stream)
{
	std::string method;
	std::vector<std::string> methods;

	while (stream >> method)
	{
		if (method.back() == ';')
		{
			method.pop_back();  // Remove trailing semicolon
			methods.push_back(method);
			break;  // End of directive
		}
		methods.push_back(method);
	}

	if (methods.empty())
		throw std::runtime_error("Expected methods after 'methods' directive");

	route.setAcceptedMethods(methods);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Accepted methods");
}

void parseRedirection(RouteConfig& route, std::istringstream& stream)
{
	std::string redirection_url;
	stream >> redirection_url;

	if (redirection_url.back() == ';')
		redirection_url.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the redirection directive");

	route.setRedirectionUrl(redirection_url);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Redirection URL: " + route.getRedirection());
}

void parseAutoindex(RouteConfig& route, std::istringstream& stream)
{
	std::string value;
	stream >> value;

	if (value.back() == ';')
		value.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the autoindex directive");

	if (value == "on")
		route.setAutoindex(true);
	else if (value == "off")
		route.setAutoindex(false);
	else
		throw std::runtime_error("Invalid autoindex value: " + value);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Autoindex: " + value);
}

void parseDefaultFile(RouteConfig& route, std::istringstream& stream)
{
	std::string default_file;
	stream >> default_file;

	if (default_file.back() == ';')
		default_file.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the default_file directive");

	route.setDefaultFile(default_file);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Default file: " + route.getDefaultFile());
}

void parseDirectoryListing(RouteConfig& route, std::istringstream& stream)
{
	std::string value;
	stream >> value;

	if (value.back() == ';')
		value.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the directory_listing directive");

	if (value == "on")
		route.setDirectoryListing(true);
	else if (value == "off")
		route.setDirectoryListing(false);
	else
		throw std::runtime_error("Invalid directory_listing value: " + value);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Directory listing: " + value);
}

void parseUploadDirectory(RouteConfig& route, std::istringstream& stream)
{
	std::string upload_directory;
	stream >> upload_directory;

	if (upload_directory.back() == ';')
		upload_directory.pop_back();  // Remove trailing semicolon
	else
		throw std::runtime_error("Expected ';' at the end of the upload_directory directive");

	route.setUploadDirectory(upload_directory);
	ensureNoTrailingTokens(stream);
	INFO("ADDED Upload directory: " + route.getUploadDirectory());
}