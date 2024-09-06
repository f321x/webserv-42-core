#include "WebServerConfig.hpp"

WebServerConfig::WebServerConfig(const std::string &config_file_path)
{
	(void)config_file_path;
}

WebServerConfig::~WebServerConfig()
{
	TRACE("WebServerConfig destructor");
}

WebServerConfig::WebServerConfig()
{
	WARN("Using dummy WebServerConfig constructor");
	_bind_ip = "0.0.0.0";
	_bind_port = 8080;
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
