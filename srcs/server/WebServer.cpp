#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config) : _config(config)
{
	SocketAddress address = config.get_bind_address();
	_bind_socket.bind_to_address(address);
	_bind_socket.listen_on_socket();
	TRACE("WebServer constructed");
}

WebServer::~WebServer()
{
}

void WebServer::serve()
{
	INFO("Webserver is running");
	// This is where the server would start serving requests
	// and responding to them.
	while (true)
	{
		TcpSocket client_socket = _bind_socket.accept_connection();
		std::string client_data = client_socket.read_client_data();
		INFO("Received data from client: " + client_data);
		sleep(1);
	}
}

// WebServer::WebServer()
// {
// 	throw std::runtime_error("Webserver cannot be instanciated without WebServerConfig");
// }

// WebServer::WebServer(const WebServer &other)
// {
// 	(void)other;
// 	throw std::runtime_error("Webserver cannot be copied");
// }

// WebServer &WebServer::operator=(const WebServer &other)
// {
// 	(void)other;
// 	throw std::runtime_error("Webserver cannot be copied");
// }
