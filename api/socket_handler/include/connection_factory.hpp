#ifndef CONNECTION_FACTORY_HPP
#define CONNECTION_FACTORY_HPP

#include <i_connection_factory.hpp>

class ConnectionFactory : public IConnectionFactory
{
public:
    ConnectionFactory(std::shared_ptr<api::socket_handler::IProxyClientFactory>);

    std::shared_ptr<IConnection> makeConnection(std::shared_ptr<IServerSocket>,
                                                int) const override;

private:
    std::shared_ptr<api::socket_handler::IProxyClientFactory> m_proxyClientFactory;
};

#endif // CONNECTION_FACTORY_HPP