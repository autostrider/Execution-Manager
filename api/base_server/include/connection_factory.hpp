#ifndef CONNECTION_FACTORY_HPP
#define CONNECTION_FACTORY_HPP

#include <i_connection_factory.hpp>
#include <i_proxy_client_factory.hpp>

using namespace api::client;
using namespace api::socket_handler;


namespace api::server
{

class ConnectionFactory : public IConnectionFactory
{
public:
    ConnectionFactory(std::shared_ptr<IProxyClientFactory>);

    std::shared_ptr<IConnection> makeConnection(std::shared_ptr<IServerSocket>,
                                                int) const override;

private:
    std::shared_ptr<IProxyClientFactory> m_proxyClientFactory;
};

}

#endif // CONNECTION_FACTORY_HPP