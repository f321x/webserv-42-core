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
#include <memory>

class WebServer
{
public:
	WebServer(const WebServerConfig &config, volatile std::sig_atomic_t *shutdown_signal);
	~WebServer();
	void serve();

private:
	// TcpSocket _bind_socket;  // commented out to prevent double destruction
	std::shared_ptr<TcpSocket> _bind_socket;
	std::vector<std::shared_ptr<TcpSocket>> _sockets; // contains all sockets including the bind socket
	WebServerConfig _config;
	volatile std::sig_atomic_t *_shutdown_signal;

	void _handle_client_data(std::shared_ptr<TcpSocket> client_socket);
	std::vector<pollfd *> _get_pollfds();
};
