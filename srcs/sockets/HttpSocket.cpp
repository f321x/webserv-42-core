#include "HttpSocket.hpp"

HttpSocket::HttpSocket(const HostPortPair &host, const WebServerConfig &config) : is_bind_socket(true)
{
	// create bind socket for the given port
	sockaddr_in address = _compose_sockaddr(host.host, host.port);
	_socket = _create_bind_socket(address);

	// store all ports listening to this port
	_available_configs = std::make_shared<std::vector<ServerConfig>>();
	for (const ServerConfig &server_config : config.getServerConfigs())
	{
		if (server_config.getPort() == host.port && server_config.getHost() == host.host)
		{
			_available_configs->push_back(server_config);
		}
	}
}

HttpSocket::HttpSocket(std::unique_ptr<TcpSocket> socket, std::shared_ptr<std::vector<ServerConfig>> available_configs) : is_bind_socket(false)
{
	_socket = std::move(socket);
	_available_configs = available_configs;
	_last_activity = std::chrono::steady_clock::now();
	request = std::make_unique<RequestPacket>(_smallest_max_body_size());
}

HttpSocket::~HttpSocket()
{
	TRACE("HttpSocket destructed");
}

HttpSocket::HttpSocket(const HttpSocket &other)
{
	_socket = std::make_unique<TcpSocket>(*other._socket);
	_available_configs = other._available_configs;
}

HttpSocket &HttpSocket::operator=(const HttpSocket &other)
{
	if (this != &other)
	{
		_socket = std::make_unique<TcpSocket>(*other._socket);
		_available_configs = other._available_configs;
	}
	return *this;
}

std::unique_ptr<HttpSocket> HttpSocket::accept_connection()
{
	std::unique_ptr<TcpSocket> new_client_socket = _socket->accept_connection();
	return std::make_unique<HttpSocket>(std::move(new_client_socket), _available_configs);
}

pollfd HttpSocket::new_pfd() const
{
	return _socket->new_pfd();
}

size_t HttpSocket::_smallest_max_body_size() const
{
	size_t smallest = INT_MAX;
	for (const ServerConfig &config : *_available_configs)
	{
		if (config.getClientMaxBodySize() < smallest)
		{
			smallest = config.getClientMaxBodySize();
		}
	}
	return smallest;
}

void HttpSocket::handle_client_data()
{
	TRACE("HANDLING CLIENT DATA");
	_last_activity = std::chrono::steady_clock::now();
	if (is_bind_socket)
		throw IsBindSocketErr("HttpSocket: Cannot handle client data on a bind socket");
	if (this->response.has_value())
		throw HttpSocketError("Response already exists, skipping handle_client_data");

	bool complete_request = false;
	try
	{
		// read the request from the client
		std::string client_data = _socket->read_once();
		// TRACE("Received data from client: " + client_data);
		// append the data to the request class
		complete_request = this->request->append(client_data);
	}
	catch (const RequestPacket::InvalidPacketException &e)
	{
		TRACE("Invalid packet received");
		this->response.emplace(bad_request());
	}
	catch (const RequestPacket::UnknownMethodException &e)
	{
		TRACE("Unknown method received");
		this->response.emplace(not_implemented()); // TODO: check which error code -> write new fixed response
	}
	catch (const RequestPacket::PayloadTooLargeException &e)
	{
		TRACE("Payload too large");
		this->response.emplace(payload_too_large());
	}
	catch (const std::exception &e)
	{
		ERROR("Reading failed: " + std::string(e.what()));
		throw ReadingFailedErr(e.what());
	}

	if (complete_request)
	{
		TRACE("Request complete, handling request");
		this->response.emplace(handle_request(*request, _available_configs)); // placing the response in optional to signal we are ready to write
		// reset this->request to a new RequestPacket for next request
		// pass max_body_size to constructor so the parsing can check if the packet is bigger than allowed
		this->request = std::make_unique<RequestPacket>(_smallest_max_body_size());
	}
}

bool HttpSocket::write_client_response()
{
	// write response to client
	try
	{
		TRACE("Sending response to client: " + response.value()->serialize());
		if (_socket->write_data(response.value()->serialize()))
		{
			bool is_final_response = response.value()->is_final_response();
			this->response.reset();
			return is_final_response;
		}
	}
	catch (const std::exception &e)
	{
		throw WritingFailedErr(e.what());
	}
	return false;
}

std::unique_ptr<TcpSocket> HttpSocket::_create_bind_socket(const sockaddr_in &address)
{
	auto new_bind_socket = std::make_unique<TcpSocket>();

	// create a socket and bind it to the configured address
	new_bind_socket->bind_to_address(address);

	// begin listening for incoming connections
	new_bind_socket->listen_on_socket();
	return new_bind_socket;
}

sockaddr_in HttpSocket::_compose_sockaddr(const std::string &addr, int port)
{
	sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = inet_addr(addr.c_str());
	return sockaddr;
}

std::chrono::steady_clock::time_point HttpSocket::last_activity() const
{
	return _last_activity;
}
