#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"

class WebServer
{
public:
	WebServer(WebServerConfig &config);
	~WebServer();
	void serve();
};
