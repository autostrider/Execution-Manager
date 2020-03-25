#include "connection_factory.hpp"
#include "connection.hpp"
#include <proxy_client_factory.hpp>

namespace base_server
{

ConnectionFactory::ConnectionFactory(std::shared_ptr<IProxyClientFactory> proxyClientFactory)
    : m_proxyClientFactory(proxyClientFactory)
    {}

std::shared_ptr<IConnection> ConnectionFactory::makeConnection(
            std::shared_ptr<IServerSocket> socket,
            int fd) const
{
    return std::make_shared<Connection>(socket, fd, m_proxyClientFactory);
}

}