#ifndef PROXY_CLIENT_FACTORY_MOCK_HPP
#define PROXY_CLIENT_FACTORY_MOCK_HPP

#include <client.hpp>
#include <i_client_factory.hpp>

using namespace api::socket_handler;

class ProxyClientFactory : public IProxyClientFactory
{
public:
    std::unique_ptr<IClient>
            makeProxyClient(const int&) const override;
};

#endif // PROXY_CLIENT_FACTORY_MOCK_HPP