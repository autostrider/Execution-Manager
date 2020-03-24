#include "connection_factory.hpp"
#include "connection.hpp"
#include <proxy_client_factory.hpp>

namespace api::server
{

ConnectionFactory::ConnectionFactory(std::shared_ptr<api::client::IProxyClientFactory> proxyClientFactory)
    : m_proxyClientFactory(proxyClientFactory)
    {}

std::shared_ptr<IConnection> ConnectionFactory::makeConnection(
            std::shared_ptr<api::socket_handler::IServerSocket> socket,
            int fd) const
{
    return std::make_shared<Connection>(socket, fd, m_proxyClientFactory);
}

}