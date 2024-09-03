#pragma once

#include "configuration.hpp"
#include "HttpPacket.hpp"

class WebServer
{
public:
	WebServer(WebServerConfig &config);
	~WebServer();
	void serve();
};
