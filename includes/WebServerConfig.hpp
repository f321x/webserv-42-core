#pragma once

#include "SocketAddress.hpp"
#include "logging.hpp"
#include <string>
#include <stdexcept>

class WebServerConfig
{
public:
	WebServerConfig(const std::string &config_file_path);
	~WebServerConfig();
	WebServerConfig(const WebServerConfig &other);
	WebServerConfig &operator=(const WebServerConfig &other);

	// should return the bind SocketAddress
	SocketAddress get_bind_address() const;

private:
	WebServerConfig();

	std::string _bind_ip = "0.0.0.0"; // default value
	uint16_t _bind_port = 8080;		  // default value
};
