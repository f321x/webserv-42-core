#pragma once

#include "WebServerConfig.hpp"
#include "logging.hpp"
#include "HttpSocket.hpp"
#include "RequestHandler.hpp"

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

const uint8_t CONNECTION_TIMEOUT = 120; // seconds

class WebServer
{
public:
	WebServer(const WebServerConfig &config);
	~WebServer();
	void serve();

private:
	std::vector<std::unique_ptr<HttpSocket>> _sockets; // contains all sockets including the bind socket
	std::vector<pollfd> _pollfds;					   // contains all pollfds for the sockets

	void _handle_client_data(std::unique_ptr<HttpSocket> &client_socket); // handle incoming client data
	void _remove_socket(int fd);										  // remove socket from _sockets and _pollfds
	void _store_socket(std::unique_ptr<HttpSocket> socket);				  // store socket in _sockets and _pollfds
	void _handle_timeouts();											  // handle timeouts for client sockets
};
