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

#pragma once

#include "configuration.hpp"
#include "http.hpp"

class WebServer
{
public:
	WebServer(WebServerConfig &config);
	~WebServer();
	void serve();
};
