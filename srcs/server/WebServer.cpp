#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config) : _config(config)
{
    // initialize fd_set
    // create a socket and bind it to the configured address
    _bind_socket.bind_to_address(config.get_bind_address());
    // begin listening for incoming connections
    _bind_socket.listen_on_socket();

    // Add the listening socket to the poll set
    _sockets.push_back(_bind_socket);

    TRACE("WebServer constructed");
}

WebServer::~WebServer()
{
}

std::vector<pollfd> WebServer::_get_pollfds()
{
    std::vector<pollfd> pfds;
    for (const auto &socket : _sockets)
    {
        pfds.push_back(socket.pfd());
    }
    return pfds;
}

void WebServer::serve()
{
    INFO("Webserver is running");
    while (true)
    {
        std::vector<pollfd> pfds = WebServer::_get_pollfds();
        int ready = poll(pfds.data(), pfds.size(), -1);
        if (ready < 0)
        {
            throw std::runtime_error("WebServer: polling file descriptors failed");
        }

        for (size_t i = 0; i < _sockets.size(); ++i)
        {
            if (_sockets[i].pfd().revents & POLLIN)
            {
                if (_sockets[i].fd() == _bind_socket.fd())
                {
                    TcpSocket new_client_socket = _bind_socket.accept_connection();
                    _sockets.push_back(new_client_socket);
                }
                else
                {
                    _handle_client_data(_sockets[i]);
                }
            }
        }
    }
}

void WebServer::_handle_client_data(TcpSocket &client_socket)
{
    try
    {
        std::string client_data = client_socket.read_client_data();
        DEBUG("Received data from client: " + client_data);
        // construct packet class with deserializer constructor
    }
    catch (const std::runtime_error &e)
    {
        // Client disconnected or error occurred
        INFO("Client disconnected: " + std::string(e.what()));
        // remove the client socket from the list of sockets
        _sockets.erase(std::remove(_sockets.begin(), _sockets.end(), client_socket), _sockets.end());
    }
}

// WebServer::WebServer()
// {
// 	throw std::runtime_error("Webserver cannot be instanciated without WebServerConfig");
// }

// WebServer::WebServer(const WebServer &other)
// {
// 	(void)other;
// 	throw std::runtime_error("Webserver cannot be copied");
// }

// WebServer &WebServer::operator=(const WebServer &other)
// {
// 	(void)other;
// 	throw std::runtime_error("Webserver cannot be copied");
// }
