#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config) : _config(config)
{
}

WebServer::~WebServer()
{
}

WebServer &WebServer::operator=(const WebServer &other)
{
	(void)other;
}
