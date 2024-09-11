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
#include <unordered_map>

class WebServer
{
public:
	WebServer(const WebServerConfig &config);
	~WebServer();
	void serve();

private:
	std::shared_ptr<TcpSocket> _bind_socket;		  // the main bind socket that listens for incoming connections
	std::vector<std::shared_ptr<TcpSocket>> _sockets; // contains all sockets including the bind socket
	std::vector<pollfd> _pollfds;					  // contains all pollfds for the sockets
	WebServerConfig _config;						  // the configuration of the server

	void _handle_client_data(std::shared_ptr<TcpSocket> client_socket); // handle incoming client data
	void _remove_socket(int fd);										// remove socket from _sockets and _pollfds
	void _store_socket(std::shared_ptr<TcpSocket> socket);				// store socket in _sockets and _pollfds
};
