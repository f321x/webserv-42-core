#include "WebServerConfig.hpp"

WebServerConfig::WebServerConfig(const std::string &config_file_path)
{
	(void)config_file_path;
}

WebServerConfig::~WebServerConfig()
{
}

WebServerConfig::WebServerConfig()
{
	throw std::runtime_error("WebServerConfig: default constructor is not allowed");
}

WebServerConfig::WebServerConfig(const WebServerConfig &other)
{
	if (this != &other)
	{
		_bind_ip = other._bind_ip;
		_bind_port = other._bind_port;
	}
}

WebServerConfig &WebServerConfig::operator=(const WebServerConfig &other)
{
	(void)other;
	return *this;
}

SocketAddress WebServerConfig::get_bind_address() const
{
	// can throw on invalid args
	SocketAddress address = SocketAddress(_bind_ip, _bind_port);
	return address;
}
