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

	// set nonblocking flag
	int flags = fcntl(_socket_fd, F_GETFL, 0);
	fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK);
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
	pfd.events = POLLIN;
	pfd.revents = 0;
	return pfd;
}

std::string TcpSocket::read_request_header()
{
	std::string result;
	char buffer[1024];
	ssize_t bytes_read;

	if (_bind_socket)
		throw std::runtime_error("TcpSocket: Cannot read data from a bind socket");

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		bytes_read = recv(_socket_fd, buffer, sizeof(buffer), 0);

		TRACE("Read " + std::to_string(bytes_read) + " bytes from client socket");
		if (bytes_read > 0)
		{
			// Data received, append to result
			result.append(buffer, bytes_read);

			// Check if we have received the full header
			if (result.find("\r\n\r\n") != std::string::npos)
				return result;
		}
		else if (bytes_read <= 0)
		{
			// Connection closed by client
			if (result.empty())
				throw std::runtime_error("TcpSocket: Connection closed by client");
			break;
		}
	}
	throw std::runtime_error("TcpSocket: failed to read request header");
}

std::string TcpSocket::read_request_body_unchunked(size_t max_body_size, size_t promised_content_length)
{
	std::string result;
	char buffer[1024];
	ssize_t bytes_read;

	result = _buffer;
	_buffer.clear();
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		bytes_read = recv(_socket_fd, buffer, sizeof(buffer), 0);

		TRACE("Read " + std::to_string(bytes_read) + " bytes from client socket");
		if (bytes_read > 0)
		{
			// Data received, append to result
			result.append(buffer, bytes_read);

			// Check if we have received the full body
			if (result.size() == promised_content_length && promised_content_length >= 0)
				break;
			else if (result.size() > promised_content_length && promised_content_length >= 0)
			{
				_buffer = result.substr(promised_content_length);
				return result.substr(0, promised_content_length);
			}

			if (result.size() > max_body_size)
				throw std::runtime_error("TcpSocket: Request body too large");
		}
		else if (bytes_read <= 0)
		{
			// Connection closed by client
			if (result.empty())
				break;
			else if (promised_content_length > 0 && result.size() < promised_content_length)
			{
				_buffer = result;
				return "";
			}
			break;
		}
	}
	return result;
}

std::pair<std::string, bool> TcpSocket::read_request_body_chunked(size_t max_body_size, std::string existing_chunked_data)
{
	std::string chunked_data;
	std::string result; // result has to be clean, unchunked content
	char buffer[1024];
	ssize_t bytes_read;

	chunked_data = existing_chunked_data;
	chunked_data.append(_buffer);
	_buffer.clear();
	while (true)
	{
		// read into buffer
		memset(buffer, 0, sizeof(buffer));
		bytes_read = recv(_socket_fd, buffer, sizeof(buffer), 0);
		if (bytes_read <= 0)
			break;

		chunked_data.append(buffer, bytes_read);
		auto [unchunked_data, complete] = _unchunk_data(chunked_data);
		result.append(unchunked_data);

		if (result.size() > max_body_size)
			throw std::runtime_error("TcpSocket: Request body too large");
		else if (complete)
			return std::make_pair(result, true);
	}
	_buffer = chunked_data;
	return std::make_pair(result, false);
}

// removes all complete chunks from the chunked_data string and returns the unchunked data
// chunked_data will be trimmed to the last incomplete chunk
// ignores prefix extensions
std::pair<std::string, bool> TcpSocket::_unchunk_data(std::string &chunked_data)
{
	std::string result;
	bool complete = false;

	while (true)
	{
		// find the end of the chunk size prefix
		size_t chunk_size_end = chunked_data.find("\r\n");
		if (chunk_size_end == std::string::npos)
			break;

		// get the chunk size
		std::string chunk_size_str = chunked_data.substr(0, chunk_size_end);
		size_t chunk_size = std::stoul(chunk_size_str, nullptr, 16);
		if (chunk_size == 0)
		{
			// end of chunked data
			complete = true;
			break;
		}

		// find the end of the chunk data
		size_t chunk_end = chunked_data.find("\r\n", chunk_size_end + 2);
		if (chunk_end == std::string::npos)
			break;

		// append the chunk data to the result
		result.append(chunked_data, chunk_size_end + 2, chunk_size);
		chunked_data = chunked_data.substr(chunk_end + 2);
	}

	return std::make_pair(result, complete);
}

void TcpSocket::write_data(const std::string &data)
{
	if (_bind_socket)
		throw std::runtime_error("TcpSocket: Cannot write data to a bind socket");

	ssize_t bytes_written = send(_socket_fd, data.c_str(), data.size(), 0);
	if (bytes_written < 0)
	{
		throw std::runtime_error("TcpSocket: failed to write data");
	}
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
