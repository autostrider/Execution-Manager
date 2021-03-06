#ifndef CONNECTION_FACTORY_MOCK_HPP
#define CONNECTION_FACTORY_MOCK_HPP

#include <i_connection_factory.hpp>

#include "gmock/gmock.h"

using namespace api;

class IConnectionFactoryMock : public IConnectionFactory
{
public:
    MOCK_METHOD(std::shared_ptr<IConnection>, makeConnection,
                (std::shared_ptr<IServerSocket>, int), (const, override));
};

#endif // CONNECTION_FACTORY_MOCK_HPP