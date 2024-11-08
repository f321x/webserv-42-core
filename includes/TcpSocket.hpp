#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "logging.hpp"
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <memory>

class TcpSocket
{
public:
	// automatically binds to the address
	TcpSocket();
	// use an existing socket file descriptor
	TcpSocket(int existing_fd);
	~TcpSocket();
	TcpSocket(const TcpSocket &other);
	bool operator==(const TcpSocket &other) const;
	bool operator!=(const TcpSocket &other) const;
	TcpSocket &operator=(const TcpSocket &other);

	void bind_to_address(const sockaddr_in &address); // bind the socket to the address
	void listen_on_socket();
	int fd() const;									// get the file descriptor
	pollfd new_pfd() const;							// get a new pollfd struct for the socket
	std::unique_ptr<TcpSocket> accept_connection(); // accept a new connection and return the newly created socket
	std::string read_once();						// Read the body of the request
	bool write_data(const std::string &data);		// write data to the socket
	bool is_bind_socket() const;					// check if the socket belongs to the main listening sockets

private:
	sockaddr_in _address;
	int _socket_fd;			   // the file descriptor of the socket
	bool _bind_socket = false; // true if the socket is the main listening socket (not a client socket)
	size_t written_bytes = 0;
};
