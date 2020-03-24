#ifndef I_SERVER_MOCK_HPP
#define I_SERVER_MOCK_HPP

#include <i_server.hpp>

#include "gmock/gmock.h"

class ServerMock : public api::server::IServer
{
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isStarted, (), (override));
    MOCK_METHOD(void, readFromSocket,
                (std::shared_ptr<api::server::IConnection>), (override));
};

#endif //I_SERVER_MOCK_HPP
