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
