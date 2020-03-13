#ifndef I_CLIENT_MOCK_HPP
#define I_CLIENT_MOCK_HPP

#include <i_client.hpp>

#include "gmock/gmock.h"

class ClientMock : public IClient
{
public:
    ClientMock() = default;
    MOCK_METHOD(void, connect, (), (override));
    MOCK_METHOD(bool, isConnected, (), (override));
    MOCK_METHOD(void, sendMessage, (const google::protobuf::Any&), (override));
    MOCK_METHOD(std::string, receive, (), (override));
    MOCK_METHOD(ssize_t, getRecvBytes, (), (override));
};

#endif // I_CLIENT_MOCK_HPP