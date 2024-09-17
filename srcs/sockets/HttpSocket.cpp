#include "HttpSocket.hpp"

HttpSocket::HttpSocket(uint16_t port, const WebServerConfig &config) : is_bind_socket(true)
{
    // create bind socket for the given port
    sockaddr_in address = _compose_sockaddr("0.0.0.0", port);
    _socket = _create_bind_socket(address);

    // store all ports listening to this port
    _available_configs = std::make_shared<std::vector<ServerConfig>>();
    for (const ServerConfig &server_config : config.getServerConfigs())
    {
        if (server_config.getPort() == port)
        {
            _available_configs->push_back(server_config);
        }
    }
}

HttpSocket::HttpSocket(std::unique_ptr<TcpSocket> socket, std::shared_ptr<std::vector<ServerConfig>> available_configs) : is_bind_socket(false)
{
    _socket = std::move(socket);
    _available_configs = available_configs;
}

HttpSocket::~HttpSocket()
{
    TRACE("HttpSocket destructed");
}

HttpSocket::HttpSocket(const HttpSocket &other)
{
    _socket = std::make_unique<TcpSocket>(*other._socket);
    _available_configs = other._available_configs;
}

HttpSocket &HttpSocket::operator=(const HttpSocket &other)
{
    if (this != &other)
    {
        _socket = std::make_unique<TcpSocket>(*other._socket);
        _available_configs = other._available_configs;
    }
    return *this;
}

std::unique_ptr<HttpSocket> HttpSocket::accept_connection()
{
    std::unique_ptr<TcpSocket> new_client_socket = _socket->accept_connection();
    return std::make_unique<HttpSocket>(std::move(new_client_socket), _available_configs);
}

pollfd HttpSocket::new_pfd() const
{
    return _socket->new_pfd();
}

void HttpSocket::handle_client_data()
{
    TRACE("HANDLING CLIENT DATA");
    if (is_bind_socket)
        throw IsBindSocketErr("HttpSocket: Cannot handle client data on a bind socket");

    // read data from client
    std::string client_data;
    try
    {
        client_data = _socket->read_client_data();
    }
    catch (const std::exception &e)
    {
        throw ReadingFailedErr(e.what());
    }

    DEBUG("Received data from client: " + client_data);
    std::unique_ptr<HttpPacket> response = handle_request(client_data, _available_configs);

    // write response to client
    try
    {
        DEBUG("Sending response to client: " + response->serializeResponse());
        _socket->write_data(response->serializeResponse());
        if (response->is_final_response())
        {
            throw IsFinalResponse("HttpSocket: Final response sent");
        }
    }
    catch (const std::exception &e)
    {
        throw WritingFailedErr(e.what());
    }
}

std::unique_ptr<TcpSocket> HttpSocket::_create_bind_socket(const sockaddr_in &address)
{
    auto new_bind_socket = std::make_unique<TcpSocket>();

    // create a socket and bind it to the configured address
    new_bind_socket->bind_to_address(address);

    // begin listening for incoming connections
    new_bind_socket->listen_on_socket();
    return new_bind_socket;
}

sockaddr_in HttpSocket::_compose_sockaddr(const std::string &addr, int port)
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    return sockaddr;
}