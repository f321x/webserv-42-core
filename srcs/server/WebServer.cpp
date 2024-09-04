#include "WebServer.hpp"

WebServer::WebServer(WebServerConfig &config) : _config(config)
{
}

WebServer::~WebServer()
{
}

void WebServer::serve()
{
	// This is where the server would start serving requests
	// and responding to them.
}

WebServer::WebServer()
{
	throw std::runtime_error("Webserver cannot be instanciated without WebServerConfig");
}

WebServer::WebServer(const WebServer &other)
{
	(void)other;
	throw std::runtime_error("Webserver cannot be copied");
}

WebServer &WebServer::operator=(const WebServer &other)
{
	(void)other;
	throw std::runtime_error("Webserver cannot be copied");
}
