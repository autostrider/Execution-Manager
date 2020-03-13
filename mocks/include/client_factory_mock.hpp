#ifndef I_CLIENT_FACTORY_MOCK_HPP
#define I_CLIENT_FACTORY_MOCK_HPP

#include <i_client_factory.hpp>

#include "gmock/gmock.h"

class IClientFactoryMock : public IClientFactory
{
public:
    IClientFactoryMock() = default;
    MOCK_CONST_METHOD1(makeClient, std::shared_ptr<Client>(const int&));
};

#endif //I_CLIENT_FACTORY_MOCK_HPP