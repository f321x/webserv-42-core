#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"
#include "logging.hpp"
#include "TcpSocket.hpp"
#include "SocketAddress.hpp"

#include <string>
#include <vector>
#include <poll.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

class WebServer
{
public:
	WebServer(const WebServerConfig &config);
	~WebServer();
	void serve();

private:
	TcpSocket _bind_socket;
	std::vector<TcpSocket> _sockets;
	WebServerConfig _config;

	void _handle_client_data(TcpSocket &client_socket);
	std::vector<pollfd> _get_pollfds();
};
