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
    TRACE("Closing sockets " + std::to_string(_sockets.size()));
    TRACE("WebServer destructed");
}

std::vector<pollfd> WebServer::_get_pollfds()
{
    std::vector<pollfd> pfds;
    for (TcpSocket &socket : _sockets)
    {
        pfds.push_back(socket.pfd());
    }
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
        int ready = poll(pfds.data(), pfds.size(), 2000);
        // TRACE("Poll returned: " + std::to_string(ready));
        if (ready < 0)
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0)
            continue;
        for (size_t i = 0; i < pfds.size(); ++i)
        {
            static int count = 0;
            count++;
            if (count > 10)
                exit(0);
            // TRACE("Checking socket " + std::to_string(pfds[i].fd) + " REVENT: " + std::to_string(pfds[i].revents));
            if (pfds[i].revents & POLLERR)
            {
                DEBUG("POLLERR detected on socket " + std::to_string(pfds[i].fd));
                _sockets.erase(std::remove(_sockets.begin(), _sockets.end(), _sockets[i]), _sockets.end()); // wonky indexing
            }
            else if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == _bind_socket.fd())
                {
                    DEBUG("Accepting new connection");
                    TcpSocket new_client_socket = _bind_socket.accept_connection();
                    _sockets.push_back(new_client_socket);
                }
                else
                {
                    DEBUG("Handling client data");
                    _handle_client_data(_sockets[i]);
                }
            }
            else if (pfds[i].revents & POLLHUP)
            {
                WARN("POLLHUP detected on socket " + std::to_string(pfds[i].fd));
                _sockets.erase(std::remove(_sockets.begin(), _sockets.end(), _sockets[i]), _sockets.end()); // wonky indexing
            }
        }
    }
}

void WebServer::_handle_client_data(TcpSocket &client_socket)
{
    DEBUG("HANDLING CLIENT DATA");
    exit(0);
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
