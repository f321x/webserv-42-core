#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

class WebServer
{
public:
	WebServer(const WebServerConfig &config);
	~WebServer();
	void serve();

private:
	WebServer();
	WebServer(const WebServer &other);
	WebServer &operator=(const WebServer &other);

	WebServerConfig _config;
};
