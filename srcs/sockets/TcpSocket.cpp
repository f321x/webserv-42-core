#include "TcpSocket.hpp"

// constructor for new client (accept_connection)
TcpSocket::TcpSocket(int existing_fd) : _socket_fd(existing_fd)
{
	_bind_socket = false;
	memset(&_address, 0, sizeof(_address));

	// set nonblocking flag
	int flags = fcntl(_socket_fd, F_GETFL, 0);
	fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK);
}

// constructor for bind socket
TcpSocket::TcpSocket()
{
	TRACE("Creating bind socket");
	_bind_socket = true;

	// open socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0)
	{
		throw std::runtime_error("TcpSocket: failed");
	}

	// Set the SO_REUSEADDR option to make the server restart faster
	int optval = 1;
	setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	// set nonblocking flag
	// int flags = fcntl(_socket_fd, F_GETFL, 0);
	fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
}

void TcpSocket::bind_to_address(const sockaddr_in &address)
{
	_address = address;
	if (bind(_socket_fd, (sockaddr *)&_address, sizeof(_address)) < 0)
	{
		close(_socket_fd);
		throw std::runtime_error("TcpSocket: failed to bind");
	}
}

bool TcpSocket::is_bind_socket() const
{
	return _bind_socket;
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

std::unique_ptr<TcpSocket> TcpSocket::accept_connection()
{
	sockaddr_in client_address;
	memset(&client_address, 0, sizeof(client_address));
	socklen_t client_address_len = sizeof(client_address);

	if (!_bind_socket)
		throw std::runtime_error("TcpSocket: Cannot accept connection on a client socket");

	int client_socket_fd = accept(_socket_fd, (sockaddr *)&client_address, &client_address_len);
	if (client_socket_fd < 0)
	{
		throw std::runtime_error("TcpSocket: failed to accept connection");
	}

	std::ostringstream ss;
	ss << "Accepted connection from address: "
	   << inet_ntoa(client_address.sin_addr)
	   << " port: " << ntohs(client_address.sin_port);
	DEBUG(ss.str());

	std::unique_ptr<TcpSocket> client_socket(new TcpSocket(client_socket_fd));
	client_socket->_address = client_address;
	return client_socket;
}

pollfd TcpSocket::new_pfd() const
{
	pollfd pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.fd = _socket_fd;
	if (this->_bind_socket)
		pfd.events = POLLIN;
	else
		pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	return pfd;
}

std::string TcpSocket::read_once()
{
	std::string result;
	char buffer[1024];
	ssize_t bytes_read;
	bytes_read = recv(_socket_fd, buffer, sizeof(buffer), 0);
	buffer[bytes_read] = '\0';

	TRACE("Read " + std::to_string(bytes_read) + " bytes from client socket");
	if (bytes_read > 0)
	{
		// Data received, append to result
		result.append(buffer, bytes_read);
	}
	else if (bytes_read == 0)
		throw ReadZeroBytesException();
	else
	{
		// Error
		throw std::runtime_error("TcpSocket: failed to read data");
	}
	return result;
}

// returnes true if finished writing
bool TcpSocket::write_data(const std::string &data)
{
	if (_bind_socket)
		throw std::runtime_error("TcpSocket: Cannot write data to a bind socket");

	std::string data_to_write;
	if (written_bytes > 0)
		data_to_write = data.substr(written_bytes);
	else
		data_to_write = data;

	int send_res = send(_socket_fd, data_to_write.c_str(), data_to_write.size(), 0);
	written_bytes += send_res;
	if (written_bytes == data.size())
	{
		written_bytes = 0;
		return true; // wrote all data
	}
	else if (send_res == 0)
	{
		return false;
	}
	else if (send_res < 0)
	{
		throw std::runtime_error("TcpSocket: failed to write data");
	}
	return false; // not finished writing
}

TcpSocket::~TcpSocket()
{
	close(_socket_fd);
	DEBUG("Closed socket fd: " + std::to_string(_socket_fd));
}

TcpSocket::TcpSocket(const TcpSocket &other) : _address(other._address), _socket_fd(other._socket_fd), _bind_socket(other._bind_socket) // _pfd(other._pfd),
{
}

TcpSocket &TcpSocket::operator=(const TcpSocket &other)
{
	_address = other._address;
	_socket_fd = other._socket_fd;
	// _pfd = other._pfd;
	_bind_socket = other._bind_socket;
	return *this;
}

int TcpSocket::fd() const
{
	return _socket_fd;
}

bool TcpSocket::operator==(const TcpSocket &other) const
{
	return _socket_fd == other._socket_fd;
}

bool TcpSocket::operator!=(const TcpSocket &other) const
{
	return !(*this == other);
}
