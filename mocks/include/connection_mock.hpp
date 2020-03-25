#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <i_connection.hpp>

#include "gmock/gmock.h"

class ConnectionMock : public IConnection
{
public:
    MOCK_METHOD(void, creatAcceptedClient, (), (override));
    MOCK_METHOD(std::string, receiveData, (), (override));
    MOCK_METHOD(int, getRecvBytes, (), (override));
};



#endif // CONNECTION_HPP