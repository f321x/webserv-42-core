#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config, volatile std::sig_atomic_t *shutdown_signal) : _config(config), _shutdown_signal(shutdown_signal)
{
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
    TRACE("WebServer destructed");
}

std::vector<pollfd> WebServer::_get_pollfds()
{
    std::vector<pollfd> pfds;
    for (TcpSocket &socket : _sockets)
    {
        pfds.push_back(socket.pfd());
    }
    // TRACE("Pollfds: " + std::to_string(pfds.size()));
    return pfds;
}

void WebServer::serve()
{
    INFO("Webserver is serving");
    while (true)
    {
        if (*_shutdown_signal)
            break;
        std::vector<pollfd> pfds = WebServer::_get_pollfds();
        TRACE("Pollfds: " + std::to_string(pfds.size()));
        int ready = poll(pfds.data(), pfds.size(), -1);
        TRACE("Poll returned: " + std::to_string(ready));
        if (ready < 0)
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0)
            continue;

        for (size_t i = 0; i < _sockets.size(); ++i)
        {
            // TRACE("Checking socket " + std::to_string(_sockets[i].fd()));
            if (pfds.data()[i].revents & POLLIN)
            {
                TRACE("IN");
                if (_sockets[i].fd() == _bind_socket.fd())
                {
                    TRACE("Accepting new connection");
                    TcpSocket new_client_socket = _bind_socket.accept_connection();
                    _sockets.push_back(new_client_socket);
                }
                else
                {
                    TRACE("Handling client data");
                    _handle_client_data(_sockets[i]);
                }

                // clear revents
                // ((_sockets[i].pfd()))->revents = 0;
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
