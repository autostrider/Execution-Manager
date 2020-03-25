#ifndef I_CLIENT_FACTORY_MOCK_HPP
#define I_CLIENT_FACTORY_MOCK_HPP

#include <i_proxy_client_factory.hpp>

#include "gmock/gmock.h"

using namespace api::client;

class ProxyClientFactoryMock : public IProxyClientFactory
{
public:
    MOCK_METHOD(std::unique_ptr<IClient>, makeProxyClient,
                (const int&), (const, override));
};

#endif //I_CLIENT_FACTORY_MOCK_HPP