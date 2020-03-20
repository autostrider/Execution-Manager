#ifndef CLIENT_FACTORY_HPP
#define CLIENT_FACTORY_HPP

#include <client.hpp>
#include <i_client_factory.hpp>

using namespace api::socket_handler;

class ProxyClientFactory : public IProxyClientFactory
{
public:
    std::unique_ptr<IClient>
            makeProxyClient(const int&) const override;
};

#endif // CLIENT_FACTORY_HPP