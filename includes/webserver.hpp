#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"
#include "iostream"

class WebServer
{
public:
	WebServer(WebServerConfig &config);
	~WebServer();
	void serve();

private:
	WebServerConfig &_config;

   	WebServer();
	WebServer(const WebServer &other);
	WebServer &operator=(const WebServer &other);
};
