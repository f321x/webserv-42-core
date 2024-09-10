#include "WebServer.hpp"

volatile std::sig_atomic_t shutdown_signal = false;
WebServer *server = nullptr;

void shutdownApplication(int signal)
{
	if (signal == SIGINT)
	{
		INFO("Shutting down application");
		shutdown_signal = true;
		exit(0);
	}
}

void cleanupExit()
{
	if (server)
	{
		delete server;
	}
}

int main(int argc, char **argv)
{
	// check arg count ./webserv configfile_path
	if (argc != 2)
	{
		ERROR("Usage: ./webserv configfile_path");
		return (1);
	}

	try
	{
		// try to parse config file
		WebServerConfig config = WebServerConfig(std::string(argv[1]));
		// init WebServer
		server = new WebServer(config, &shutdown_signal);
	}
	catch (std::exception &e)
	{
		ERROR(e.what());
		return (1);
	}
	atexit(cleanupExit);
	signal(SIGINT, shutdownApplication);

	// start server
	server->serve();
	delete server;

	return 0;
}

// / working demo server
// int main(void)
// {
// 	int bind_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (bind_socket_fd < 0)
// 	{
// 		throw std::runtime_error("TcpSocket: failed");
// 	}

// 	// set nonblocking flag
// 	int flags = fcntl(bind_socket_fd, F_GETFL, 0);
// 	fcntl(bind_socket_fd, F_SETFL, flags | O_NONBLOCK);

// 	// set pollfd struct
// 	pollfd pfd;
// 	memset(&pfd, 0, sizeof(pfd));
// 	pfd.fd = bind_socket_fd;
// 	pfd.events = POLLIN;
// 	pfd.revents = 0;

// 	std::vector<pollfd> pfds;
// 	pfds.push_back(pfd);

// 	// define address
// 	sockaddr_in address;
// 	memset(&address, 0, sizeof(address));
// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = inet_addr("0.0.0.0");
// 	if (address.sin_addr.s_addr == INADDR_NONE)
// 	{
// 		close(bind_socket_fd);
// 		throw std::runtime_error("TcpSocket: invalid address");
// 	}
// 	address.sin_port = htons(8080);

// 	// bind to address
// 	if (bind(bind_socket_fd, (sockaddr *)&address, sizeof(address)) < 0)
// 	{
// 		close(bind_socket_fd);
// 		throw std::runtime_error("TcpSocket: failed to bind");
// 	}

// 	// listen on socket
// 	if (listen(bind_socket_fd, 256) < 0)
// 	{
// 		close(bind_socket_fd);
// 		throw std::runtime_error("TcpSocket: failed to listen");
// 	}

// 	while (true)
// 	{
// 		int ready = poll(pfds.data(), pfds.size(), 2000);
// 		if (ready < 0)
// 			throw std::runtime_error("WebServer: polling file descriptors failed");
// 		else if (ready == 0)
// 			continue;

// 		for (size_t i = 0; i < pfds.size(); ++i)
// 		{
// 			if (pfds[i].revents & POLLIN)
// 			{
// 				if (pfds[i].fd == bind_socket_fd)
// 				{
// 					sockaddr_in client_address;
// 					memset(&client_address, 0, sizeof(client_address));
// 					socklen_t client_address_len = sizeof(client_address);
// 					int client_socket_fd = accept(bind_socket_fd, (sockaddr *)&client_address, &client_address_len);
// 					if (client_socket_fd < 0)
// 					{
// 						throw std::runtime_error("TcpSocket: failed to accept connection");
// 					}

// 					std::ostringstream ss;
// 					ss << "Accepted connection from address: "
// 					   << inet_ntoa(client_address.sin_addr)
// 					   << " port: " << ntohs(client_address.sin_port);
// 					DEBUG(ss.str());

// 					// set nonblocking flag
// 					int flags = fcntl(client_socket_fd, F_GETFL, 0);
// 					fcntl(client_socket_fd, F_SETFL, flags | O_NONBLOCK);

// 					// set pollfd struct
// 					pollfd client_pfd;
// 					memset(&client_pfd, 0, sizeof(client_pfd));
// 					client_pfd.fd = client_socket_fd;
// 					client_pfd.events = POLLIN;
// 					client_pfd.revents = 0;

// 					pfds.push_back(client_pfd);
// 				}
// 				else
// 				{
// 					std::string result;
// 					char buffer[1024];
// 					memset(buffer, 0, 1024);
// 					ssize_t bytes_read = 0;

// 					while (true)
// 					{
// 						TRACE("Reading data from client");
// 						bytes_read = recv(pfds[i].fd, buffer, sizeof(buffer), 0);

// 						if (bytes_read > 0)
// 						{
// 							// Data received, append to result
// 							result.append(buffer, bytes_read);
// 							memset(buffer, 0, 1024);
// 							// TRACE("Data received: " + result);
// 						}
// 						else if (bytes_read == 0)
// 						{
// 							// Connection closed by client
// 							if (result.empty())
// 							{
// 								close(pfds[i].fd);
// 								DEBUG("TcpSocket: Connection closed by client");
// 							}
// 							break;
// 						}
// 						else
// 						{
// 							// ERROR("TcpSocket: Error reading data from socket bytes_read: " + std::string(strerror(errno)) + " -> READING: " + std::to_string(pfds[i].revents));
// 							// std::cerr << "Errno: " << errno << std::endl;
// 							close(pfds[i].fd);
// 							pfds.erase(pfds.begin() + i);
// 							break;
// 						}
// 					}
// 					DEBUG("RECEIVED: " + result);
// 				}
// 			}
// 		}
// 	}
// }
