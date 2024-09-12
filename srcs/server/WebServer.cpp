#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config) : _config(config)
{
    // reserve memory for the vecs to increase performance by preventing reallocation
    _sockets.reserve(256);
    _pollfds.reserve(256);

    // find unique ports
    std::unordered_set<uint16_t> unique_ports;
    for (const ServerConfig &server_config : config.getServerConfigs())
        unique_ports.insert(server_config.getPort());

    // create a new bind sockets for each port in configuration
    for (int port : unique_ports)
    {
        sockaddr_in server_address = _compose_sockaddr("0.0.0.0", port);
        std::shared_ptr<TcpSocket> _new_bind_socket = _create_bind_socket(server_address);
        // Add the listening socket to the poll set
        _store_socket(_new_bind_socket);
    }

    TRACE("WebServer constructed");
}

std::shared_ptr<TcpSocket> WebServer::_create_bind_socket(const sockaddr_in &address)
{
    std::shared_ptr<TcpSocket> _new_bind_socket(new TcpSocket());

    // create a socket and bind it to the configured address
    _new_bind_socket->bind_to_address(address);

    // begin listening for incoming connections
    _new_bind_socket->listen_on_socket();

    return _new_bind_socket;
}

WebServer::~WebServer()
{
    TRACE("Closing sockets " + std::to_string(_sockets.size()));
    TRACE("WebServer destructed");
}

void WebServer::_remove_socket(int fd)
{
    auto it = std::find_if(_pollfds.begin(), _pollfds.end(),
                           [fd](const pollfd &pfd)
                           { return pfd.fd == fd; });

    if (it != _pollfds.end())
    {
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
        // poll the sockets for incoming data (pointer to first element, number of elements, timeout)
        int ready = poll(_pollfds.data(), _pollfds.size(), -1);
        TRACE("Poll returned: " + std::to_string(ready));

        if (ready < 0) // error
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0) // no new events
            continue;

        // iterate trough the pollfds and handle available events
        for (size_t i = 0; i < _pollfds.size(); ++i) // wonky indexing
        {
            TRACE("Checking socket " + std::to_string(_pollfds[i].fd) + " REVENT: " + std::to_string(_pollfds[i].revents));

            if (_pollfds[i].revents & POLLIN) // POLLIN is set if there is data to read
            {
                if (_sockets[i]->is_bind_socket()) // if the bind socket has data to read its a new connection
                {
                    TRACE("Accepting new connection");
                    std::shared_ptr<TcpSocket> new_client_socket = _sockets[i]->accept_connection(); // accept the connection, return new socket
                    _store_socket(new_client_socket);                                                // store new socket in the list of sockets
                    break;                                                                           // may uneccessary, investigate later
                }
                else
                {
                    TRACE("Handling client data");
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
    TRACE("HANDLING CLIENT DATA");
    try
    {
        std::string client_data = client_socket->read_client_data();
        DEBUG("Received data from client: " + client_data);
        client_socket->write_data(std::string("HTTP/1.1 200 OK\r\n\r\nECHO: [\n") + client_data + std::string("]\n"));
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

sockaddr_in WebServer::_compose_sockaddr(const std::string &addr, int port)
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    return sockaddr;
}
