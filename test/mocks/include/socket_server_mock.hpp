#ifndef EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP
#define EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP

#include "i_socket_server.hpp"

#include "gmock/gmock.h"

class SocketServerMock: public ISocketServer
{
public:
  SocketServerMock() = default;
  MOCK_METHOD0(getData, std::string());
  MOCK_METHOD0(startServer, void());
  MOCK_METHOD0(closeServer, void());
};

#endif //EXECUTION_MANAGER_SOCKET_SERVER_MOCK_HPP
