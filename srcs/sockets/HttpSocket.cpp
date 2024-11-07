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

int HttpSocket::_smallest_max_body_size() const
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

	std::shared_ptr<ResponsePacket> response = nullptr;
	// read data from client
	std::string client_data;
	try
	{
		if (!_ongoing_chunked_request)
		{
			std::string partial_packet = _socket->read_request_header();
			std::shared_ptr<RequestPacket> header_only_packet = std::make_shared<RequestPacket>(partial_packet);
			client_data += partial_packet;

			// deduct the bytes we already read in excess (bytes after the header) from the promised content length
			int remaining_bytes = header_only_packet->getContentLengthHeader() - header_only_packet->getContentSize();

			if (header_only_packet->getContentLengthHeader() > _smallest_max_body_size())
				return _write_client_response(payload_too_large());
			else if (header_only_packet->isChunked())
			{
				DEBUG("PARTIAL PACKET CONTENT: " + header_only_packet->getContent());
				remove_content_from_packet(client_data, header_only_packet->getContent());
				auto [unchunked_content, finished] = _socket->read_request_body_chunked(_smallest_max_body_size(), header_only_packet->getContent());
				if (finished)
					client_data += unchunked_content;
				else
				{
					_chunked_packet_buffer += client_data;
					_chunked_packet_buffer += unchunked_content;
					_ongoing_chunked_request = true;
					return;
				}
			}
			else
				client_data += _socket->read_request_body_unchunked(_smallest_max_body_size(), remaining_bytes);
		}
		else // directly read chunked data because the header and existing content is already in the buffer
		{
			auto [unchunked_content, finished] = _socket->read_request_body_chunked(_smallest_max_body_size(), "");
			if (finished)
			{
				client_data += _chunked_packet_buffer;
				client_data += unchunked_content;
				_chunked_packet_buffer.clear();
				_ongoing_chunked_request = false;
			}
			else
			{
				_chunked_packet_buffer += unchunked_content;
				return;
			}
		}
	}
	catch (const std::exception &e)
	{
		throw ReadingFailedErr(e.what());
	}

	DEBUG("Received data from client: " + client_data);
	response = handle_request(client_data, _available_configs);

	return _write_client_response(std::move(response));
}

void HttpSocket::_write_client_response(std::shared_ptr<ResponsePacket> response)
{
	// write response to client
	try
	{
		TRACE("Sending response to client: " + response->serialize());
		_socket->write_data(response->serialize());
		if (response->is_final_response())
		{
			throw IsFinalResponse("HttpSocket: Final response sent");
		}
	}
	catch (const std::exception &e)
	{
		throw WritingFailedErr(e.what());
	}
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
