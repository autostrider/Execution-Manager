#ifndef PROXY_CLIENT_FACTORY_MOCK_HPP
#define PROXY_CLIENT_FACTORY_MOCK_HPP

#include <client.hpp>
#include <i_proxy_client_factory.hpp>

namespace base_client
{

class ProxyClientFactory : public IProxyClientFactory
{
public:
    std::unique_ptr<IClient>
            makeProxyClient(const int&) const override;
};

}
#endif // PROXY_CLIENT_FACTORY_MOCK_HPP