#ifndef EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP
#define EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP

#include "i_socket_server.hpp"

#include "gmock/gmock.h"

class SocketServerMock: public ISocketServer
{
public:
  SocketServerMock() = default;
  MOCK_METHOD(std::string, getData, ());
  MOCK_METHOD(void, startServer, ());
  MOCK_METHOD(void, closeServer, ());
};

#endif //EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP
