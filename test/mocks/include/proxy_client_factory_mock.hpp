#ifndef PROXY_CLIENT_FACTORY_MOCK_HPP
#define PROXY_CLIENT_FACTORY_MOCK_HPP

#include <i_proxy_client_factory.hpp>

#include "gmock/gmock.h"

using namespace api;

class ProxyClientFactoryMock : public IProxyClientFactory
{
public:
    MOCK_METHOD(std::unique_ptr<IClient>, makeProxyClient,
                (const int&), (const, override));
};

#endif //PROXY_CLIENT_FACTORY_MOCK_HPP