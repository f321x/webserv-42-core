#include "WebServer.hpp"

WebServer::WebServer(const WebServerConfig &config)
{
    // reserve memory for the vecs to increase performance by preventing reallocation
    _sockets.reserve(512);
    _pollfds.reserve(512);

    // find unique ports
    std::unordered_set<HostPortPair, HostPortPairHash> unique_hosts;
    for (const ServerConfig &server_config : config.getServerConfigs())
    {
        int port = server_config.getPort();
        if (port < 0 || port > 65535)
            throw std::runtime_error("Port number " + std::to_string(port) + " is out of the valid range for an unsigned short.");
        unsigned short ushort_port = static_cast<unsigned short>(port);
        unique_hosts.insert(HostPortPair{server_config.getHost(), ushort_port});
    }

    // create a new bind sockets for each port in configuration
    for (HostPortPair host : unique_hosts)
    {
        INFO("Creating bind socket for " + host.host + ":" + std::to_string(host.port));
        auto new_bind_socket = std::make_unique<HttpSocket>(host, config);
        // Add the listening socket to the poll set
        _store_socket(std::move(new_bind_socket));
    }

    TRACE("WebServer constructed");
}

WebServer::~WebServer()
{
    TRACE("Closing sockets " + std::to_string(_sockets.size()));
    TRACE("WebServer destructed");
}

// main loop of the server
// poll for new events on bind sockets and handle events accordingly
void WebServer::serve()
{
    INFO("Webserver is serving");
    while (true)
    {
        _handle_timeouts();
        // poll the sockets for incoming data (pointer to first element, number of elements, timeout)
        int ready = poll(_pollfds.data(), _pollfds.size(), 2000);
        // TRACE("Poll returned: " + std::to_string(ready));

        if (ready < 0) // error
            throw std::runtime_error("WebServer: polling file descriptors failed");
        else if (ready == 0) // no new events
            continue;

        // iterate trough the pollfds and handle available events
        for (size_t i = 0; i < _pollfds.size(); ++i) // wonky indexing
        {
            // TRACE("Checking socket " + std::to_string(_pollfds[i].fd) + " REVENT: " + std::to_string(_pollfds[i].revents));

            if (_pollfds[i].revents & POLLIN) // POLLIN is set if there is data to read
            {
                if (_sockets[i]->is_bind_socket) // if the bind socket has data to read its a new connection
                {
                    TRACE("Accepting new connection");
                    std::unique_ptr<HttpSocket> new_client_socket = _sockets[i]->accept_connection(); // accept the connection, return new socket
                    _store_socket(std::move(new_client_socket));                                      // store new socket in the list of sockets
                    break;                                                                            // may uneccessary, investigate later
                }
                else
                {
                    TRACE("Handling client data");
                    try
                    {
                        _sockets[i]->handle_client_data(); // handle the client data
                    }
                    catch (ReadingFailedErr &e)
                    {
                        DEBUG("Reading failed: " + std::string(e.what()));
                        _remove_socket(_pollfds[i].fd);
                    }
                    catch (WritingFailedErr &e)
                    {
                        DEBUG(std::string(e.what()));
                        _remove_socket(_pollfds[i].fd);
                    }
                    catch (IsFinalResponse) // may not the most elegant solution
                    {
                        _remove_socket(_pollfds[i].fd);
                    }
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

// remove a socket from both the pollfds and sockets vecs
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

// store a socket in both the pollfds and sockets vecs
void WebServer::_store_socket(std::unique_ptr<HttpSocket> socket)
{
    _pollfds.push_back(socket->new_pfd());
    _sockets.push_back(std::move(socket));
}

void WebServer::_handle_timeouts()
{
    for (size_t i = 0; i < _sockets.size(); ++i)
    {
        if (!_sockets[i]->is_bind_socket && std::chrono::steady_clock::now() - _sockets[i]->last_activity() > std::chrono::seconds(CONNECTION_TIMEOUT))
        {
            DEBUG("Client socket timed out");
            _remove_socket(_pollfds[i].fd);
        }
    }
}
