#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config, volatile std::sig_atomic_t *shutdown_signal) : _config(config), _shutdown_signal(shutdown_signal)
{
    std::shared_ptr<TcpSocket> _new_bind_socket(new TcpSocket());
    _bind_socket = _new_bind_socket;

    // create a socket and bind it to the configured address
    _bind_socket->bind_to_address(config.get_bind_address());
    // begin listening for incoming connections
    _bind_socket->listen_on_socket();

    // reserve memory for the vecs to increase performance by preventing reallocation
    _sockets.reserve(256);
    _pollfds.reserve(256);

    // Add the listening socket to the poll set
    _pollfds.push_back(_bind_socket->new_pfd());
    _sockets.push_back(_bind_socket);

    TRACE("WebServer constructed");
}

WebServer::~WebServer()
{
    TRACE("Closing sockets " + std::to_string(_sockets.size()));
    TRACE("WebServer destructed");
}

void WebServer::_remove_socket(int fd)
{
    auto it = std::find_if(_pollfds.begin(), _pollfds.end(),
                        [fd](const pollfd& pfd) { return pfd.fd == fd; });
        
    if (it != _pollfds.end()) {
        size_t index = std::distance(_pollfds.begin(), it);
        _pollfds.erase(it);
        _sockets.erase(_sockets.begin() + index);
    }
}

void WebServer::_store_socket(std::shared_ptr<TcpSocket> socket)
{
    _sockets.push_back(socket);
    _pollfds.push_back(socket->new_pfd());
}

void WebServer::serve()
{
    INFO("Webserver is serving");
    while (true)
    {
        // if shutdown signal is set by sigint handler the loop ends to close all sockets
        if (*_shutdown_signal)
            break;

        // poll the sockets for incoming data (pointer to first element, number of elements, timeout)
        int ready = poll(_pollfds.data(), _pollfds.size(), -1);
        DEBUG("Poll returned: " + std::to_string(ready));

        if (ready < 0) // error
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0) // no new events
            continue;

        // iterate trough the pollfds and handle available events
        for (size_t i = 0; i < _pollfds.size(); ++i)  // wonky indexing 
        {
            TRACE("Checking socket " + std::to_string(_pollfds[i].fd) + " REVENT: " + std::to_string(_pollfds[i].revents));

            if (_pollfds[i].revents & POLLIN) // POLLIN is set if there is data to read
            {
                if (_pollfds[i].fd == _bind_socket->fd()) // if the bind socket has data to read its a new connection
                {
                    DEBUG("Accepting new connection");
                    std::shared_ptr<TcpSocket> new_client_socket = _bind_socket->accept_connection(); // accept the connection, return new socket
                    _store_socket(new_client_socket);                                            // store new socket in the list of sockets
                    break;                                                                            // may uneccessary, investigate later
                }
                else
                {
                    DEBUG("Handling client data");
                    _handle_client_data(_sockets[i]);
                }
            }
            else if (_pollfds[i].revents & POLLHUP)
            {
                WARN("POLLHUP detected on socket " + std::to_string(_pollfds[i].fd));
                _remove_socket(_pollfds[i].fd);
            }
            else if (_pollfds[i].revents & POLLERR)
            {
                WARN("POLLERR detected on socket " + std::to_string(_pollfds[i].fd));
                _remove_socket(_pollfds[i].fd);
            }
        }
    }
}

void WebServer::_handle_client_data(std::shared_ptr<TcpSocket> client_socket)
{
    DEBUG("HANDLING CLIENT DATA");
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
        _remove_socket(client_socket->fd());
    }
}