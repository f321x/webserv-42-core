#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"
#include "logging.hpp"
#include "TcpSocket.hpp"
#include "SocketAddress.hpp"

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
	WebServerConfig _config;
	TcpSocket _bind_socket;

	// WebServer();
	// WebServer(const WebServer &other);
	// WebServer &operator=(const WebServer &other);
};
