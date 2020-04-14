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
    MOCK_METHOD(ssize_t, receive, (std::string&), (override));
    MOCK_METHOD(ssize_t, getRecvBytes, (), (override));
    MOCK_METHOD(void, sendMessage, (const google::protobuf::Message& context), (override));
    MOCK_METHOD(int, getClientFd, (), (override));
};

#endif // CLIENT_MOCK_HPP