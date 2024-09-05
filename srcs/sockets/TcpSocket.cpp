#include "TcpSocket.hpp"

TcpSocket::TcpSocket(int existing_fd) : _socket_fd(existing_fd)
{
}

TcpSocket::TcpSocket()
{
	_bind_socket = true;
	// open socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0)
	{
		throw std::runtime_error("TcpSocket: failed");
	}
}

void TcpSocket::bind_to_address(const SocketAddress &address)
{
	_address = address.get_sockaddr();
	if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		close(_socket_fd);
		throw std::runtime_error("TcpSocket: failed to bind");
	}
}

void TcpSocket::listen_on_socket()
{
	if (listen(_socket_fd, 256) < 0)
	{
		close(_socket_fd);
		throw std::runtime_error("TcpSocket: failed to listen");
	}

	std::ostringstream ss;
	ss << "Listening on address: "
	   << inet_ntoa(_address.sin_addr)
	   << " port: " << ntohs(_address.sin_port);
	DEBUG(ss.str());
}

TcpSocket TcpSocket::accept_connection()
{
	sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	int client_socket_fd = accept(_socket_fd, (sockaddr *)&client_address, &client_address_len);
	if (client_socket_fd < 0)
	{
		close(_socket_fd);
		throw std::runtime_error("TcpSocket: failed to accept connection");
	}

	std::ostringstream ss;
	ss << "Accepted connection from address: "
	   << inet_ntoa(client_address.sin_addr)
	   << " port: " << ntohs(client_address.sin_port);
	DEBUG(ss.str());

	TcpSocket client_socket = TcpSocket(client_socket_fd);
	client_socket._address = client_address;
	return client_socket;
}

std::string TcpSocket::read_client_data()
{
	std::string result;
	char buffer[1024];
	ssize_t bytes_read;

	if (_bind_socket)
	{
		throw std::runtime_error("TcpSocket: Cannot read data from a bind socket");
	}

	while (true)
	{
		TRACE("Reading data from client");
		bytes_read = recv(_socket_fd, buffer, sizeof(buffer), 0);

		if (bytes_read > 0)
		{
			// Data received, append to result
			result.append(buffer, bytes_read);
		}
		else if (bytes_read == 0)
		{
			// Connection closed by client
			if (result.empty())
			{
				close(_socket_fd);
				throw std::runtime_error("TcpSocket: Connection closed by client");
			}
			break;
		}
		else
		{
			close(_socket_fd);
			throw std::runtime_error("TcpSocket: Error reading data from socket");
		}
	}
	DEBUG("Read data from client: " + result);
	return result;
}

TcpSocket::~TcpSocket()
{
	close(_socket_fd);
}

TcpSocket::TcpSocket(const TcpSocket &other) : _address(other._address), _socket_fd(other._socket_fd)
{
}

TcpSocket &TcpSocket::operator=(const TcpSocket &other)
{
	_address = other._address;
	_socket_fd = other._socket_fd;
	return *this;
}
