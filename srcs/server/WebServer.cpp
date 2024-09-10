#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config, volatile std::sig_atomic_t *shutdown_signal) : _config(config), _shutdown_signal(shutdown_signal)
{
    std::shared_ptr<TcpSocket> _new_bind_socket(new TcpSocket());
    _bind_socket = _new_bind_socket;

    // create a socket and bind it to the configured address
    _bind_socket->bind_to_address(config.get_bind_address());
    // begin listening for incoming connections
    _bind_socket->listen_on_socket();

    // Add the listening socket to the poll set
    _sockets.push_back(_bind_socket);

    TRACE("WebServer constructed");
}

WebServer::~WebServer()
{
    TRACE("Closing sockets " + std::to_string(_sockets.size()));
    TRACE("WebServer destructed");
}

std::vector<pollfd *> WebServer::_get_pollfds()
{
    std::vector<pollfd *> pfds;
    for (auto socket : _sockets)
    {
        pfds.push_back(socket->pfd());
    }
    if (pfds.empty())
        throw std::runtime_error("WebServer: no sockets to poll");
    return pfds;
}

void WebServer::serve()
{
    INFO("Webserver is serving");
    while (true)
    {
        // if shutdown signal is set by sigint handler the loop ends to close all sockets
        if (*_shutdown_signal)
            break;

        // fetch pollfd pointers from the sockets
        std::vector<pollfd *> pfds = WebServer::_get_pollfds();

        // poll the sockets for incoming data (pointer to first element, number of elements, timeout)
        int ready = poll(pfds[0], pfds.size(), 2000);
        DEBUG("Poll returned: " + std::to_string(ready));

        if (ready < 0) // error
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0) // no new events
            continue;

        // iterate trough the pollfds and handle available events
        for (size_t i = 0; i < _sockets.size(); ++i)
        {
            TRACE("Checking socket " + std::to_string(_sockets[i]->fd()) + " REVENT: " + std::to_string(_sockets[i]->pfd()->revents));

            if (_sockets[i]->pfd()->revents & POLLIN) // POLLIN is set if there is data to read
            {
                if (_sockets[i]->fd() == _bind_socket->fd()) // if the bind socket has data to read its a new connection
                {
                    DEBUG("Accepting new connection");
                    std::shared_ptr<TcpSocket> new_client_socket = _bind_socket->accept_connection(); // accept the connection, return new socket
                    _sockets.push_back(new_client_socket);                                            // store new socket in the list of sockets
                }
                else
                {
                    DEBUG("Handling client data");
                    _handle_client_data(_sockets[i]);
                }
            }
            else if (_sockets[i]->pfd()->revents & POLLHUP)
            {
                WARN("POLLHUP detected on socket " + std::to_string(_sockets[i]->fd()));
                _sockets.erase(std::remove(_sockets.begin(), _sockets.end(), _sockets[i]), _sockets.end());
            }
            else if (_sockets[i]->pfd()->revents & POLLERR)
            {
                WARN("POLLERR detected on socket " + std::to_string(_sockets[i]->fd()));
                _sockets.erase(std::remove(_sockets.begin(), _sockets.end(), _sockets[i]), _sockets.end());
            }
        }
    }
}

void WebServer::_handle_client_data(std::shared_ptr<TcpSocket> client_socket)
{
    DEBUG("HANDLING CLIENT DATA");
    exit(0);
    try
    {
        std::string client_data = client_socket->read_client_data();
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
