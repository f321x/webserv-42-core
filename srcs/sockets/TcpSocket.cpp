#include "TcpSocket.hpp"

TcpSocket::TcpSocket()
{
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
		throw std::runtime_error("TcpSocket: failed to bind");
	}
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
