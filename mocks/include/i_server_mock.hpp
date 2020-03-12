#ifndef I_SERVER_MOCK_HPP
#define I_SERVER_MOCK_HPP

#include <i_server.hpp>

#include "gmock/gmock.h"

class ServerMock : public IServer
{
public:
    ServerMock() = default;
    MOCK_METHOD0(bool, start, ());
    MOCK_METHOD0(void, stop, ());
    MOCK_METHOD0(bool, isStarted, ());
    MOCK_METHOD0(int, acceptConnection, ());
    MOCK_METHOD1(void, readFromSocket, (std::shared_ptr<Client>));
};

#endif //I_SERVER_MOCK_HPP
