#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <i_connection.hpp>

#include "gmock/gmock.h"

class ConnectionMock : public api::IConnection
{
public:
    MOCK_METHOD(void, creatAcceptedClient, (), (override));
    MOCK_METHOD(int, receiveData, (std::string&), (override));
    MOCK_METHOD(int, getRecvBytes, (), (override));
    MOCK_METHOD(void, sendData,(const google::protobuf::Message&), (override));
};



#endif // CONNECTION_HPP