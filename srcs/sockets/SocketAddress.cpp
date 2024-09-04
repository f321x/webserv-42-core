#include "SocketAddress.hpp"

SocketAddress::SocketAddress(const std::string &address, uint16_t port)
{
	if (address.empty())
		throw std::runtime_error("SocketAddress: address is empty");
	if (port == 0)
		throw std::runtime_error("SocketAddress: port is 0");

	_sockaddr.sin_family = AF_INET;	  // AF_INET is the address family for IPv4
	_sockaddr.sin_port = htons(port); // htons converts the port number to network byte order

	_sockaddr.sin_addr.s_addr = inet_addr(address.c_str()); // inet_addr converts the IP address to a binary form
	if (_sockaddr.sin_addr.s_addr == INADDR_NONE)
		throw std::runtime_error("SocketAddress: invalid address");
}

SocketAddress::~SocketAddress()
{
}

SocketAddress::SocketAddress(const SocketAddress &other)
{
	_sockaddr = other._sockaddr;
}

SocketAddress &SocketAddress::operator=(const SocketAddress &other)
{
	_sockaddr = other._sockaddr;
	return *this;
}

const sockaddr_in &SocketAddress::get_sockaddr() const
{
	return _sockaddr;
}
