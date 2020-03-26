#ifndef CLIENT_MOCK_HPP
#define CLIENT_MOCK_HPP

#include <i_client.hpp>

#include "gmock/gmock.h"

using namespace api;

class ClientMock : public IClient
{
public:
    MOCK_METHOD(void, connect, (), (override));
    MOCK_METHOD(bool, isConnected, (), (override));
    MOCK_METHOD(void, sendMessage, (const google::protobuf::Any&), (override));
    MOCK_METHOD(std::string, receive, (), (override));
    MOCK_METHOD(ssize_t, getRecvBytes, (), (override));
};

#endif // CLIENT_MOCK_HPP