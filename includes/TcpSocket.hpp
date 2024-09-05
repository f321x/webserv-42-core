#pragma once

#include "SocketAddress.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "logging.hpp"

class TcpSocket
{
public:
	// automatically binds to the address
	TcpSocket();
	// use an existing socket file descriptor
	TcpSocket(int existing_fd);
	~TcpSocket();
	TcpSocket(const TcpSocket &other);
	TcpSocket &operator=(const TcpSocket &other);

	void bind_to_address(const SocketAddress &address);
	void listen_on_socket();
	TcpSocket accept_connection();
	std::string read_client_data();

private:
	sockaddr_in _address;
	int _socket_fd;
	bool _bind_socket = false;
};
