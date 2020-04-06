#ifndef I_SERVER_MOCK_HPP
#define I_SERVER_MOCK_HPP

#include <i_server.hpp>

#include "gmock/gmock.h"

class ServerMock : public api::IServer
{
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isStarted, (), (override));
    MOCK_METHOD(void, readFromSocket,
                (std::shared_ptr<api::IConnection>), (override));
    MOCK_METHOD(bool, getQueueElement, (std::string& data), (override));
};

#endif //I_SERVER_MOCK_HPP
