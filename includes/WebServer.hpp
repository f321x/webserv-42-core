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
#include <csignal>

class WebServer
{
public:
	WebServer(const WebServerConfig &config, volatile std::sig_atomic_t *shutdown_signal);
	~WebServer();
	void serve();

private:
	TcpSocket _bind_socket;
	std::vector<TcpSocket> _sockets;
	WebServerConfig _config;
	volatile std::sig_atomic_t *_shutdown_signal;

	void _handle_client_data(TcpSocket &client_socket);
	std::vector<pollfd> _get_pollfds();
};
