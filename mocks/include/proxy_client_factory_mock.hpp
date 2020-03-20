#ifndef I_CLIENT_FACTORY_MOCK_HPP
#define I_CLIENT_FACTORY_MOCK_HPP

#include <i_client_factory.hpp>

#include "gmock/gmock.h"

class ProxyClientFactoryMock : public api::socket_handler::IProxyClientFactory
{
public:
    ~ProxyClientFactoryMock() = default;
    MOCK_METHOD(std::unique_ptr<api::socket_handler::IClient>, makeProxyClient,
                (const int&), (const, override));
};

#endif //I_CLIENT_FACTORY_MOCK_HPP