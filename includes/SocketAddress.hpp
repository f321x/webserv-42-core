#pragma once

#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

// constructor can throw and should be caught by the caller
class SocketAddress
{
public:
	SocketAddress(const std::string &ip, uint16_t port);
	~SocketAddress();
	SocketAddress(const SocketAddress &other);
	SocketAddress &operator=(const SocketAddress &other);

	const sockaddr_in &get_sockaddr() const;

private:
	SocketAddress();
	sockaddr_in _sockaddr;
};

// struct sockaddr_in
// {
// 	sa_family_t sin_family;	 /* AF_INET */
// 	in_port_t sin_port;		 /* Port number */
// 	struct in_addr sin_addr; /* IPv4 address */
// };
