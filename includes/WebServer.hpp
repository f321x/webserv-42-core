#pragma once

#include "WebServerConfig.hpp"
#include "HttpPacket.hpp"
#include "logging.hpp"
#include "TcpSocket.hpp"

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <poll.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <csignal>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class WebServer
{
public:
	WebServer(const WebServerConfig &config);
	~WebServer();
	void serve();

private:
	// std::vector<std::shared_ptr<TcpSocket>> _bind_sockets; // the main bind socket that listens for incoming connections
	std::vector<std::shared_ptr<TcpSocket>> _sockets; // contains all sockets including the bind socket
	std::vector<pollfd> _pollfds;					  // contains all pollfds for the sockets
	WebServerConfig _config;						  // the configuration of the server

	std::shared_ptr<TcpSocket> _create_bind_socket(const sockaddr_in &address); // create a new bind socket
	void _handle_client_data(std::shared_ptr<TcpSocket> client_socket);			// handle incoming client data
	void _remove_socket(int fd);												// remove socket from _sockets and _pollfds
	void _store_socket(std::shared_ptr<TcpSocket> socket);						// store socket in _sockets and _pollfds
	sockaddr_in _compose_sockaddr(const std::string &addr, int port);			// compose a sockaddr_in struct
};
