#pragma once

#include "SocketAddress.hpp"
#include <sys/socket.h>
#include <unistd.h>

class TcpSocket
{
public:
	// automatically binds to the address
	TcpSocket();
	~TcpSocket();
	TcpSocket(const TcpSocket &other);
	TcpSocket &operator=(const TcpSocket &other);

	void bind_to_address(const SocketAddress &address);

private:
	sockaddr_in _address;
	int _socket_fd;
};
