#ifndef I_SOCKET_INTERFACE_MOCK_HPP
#define I_SOCKET_INTERFACE_MOCK_HPP

#include "i_socket_interface.hpp"

#include "gmock/gmock.h"

class SocketInterfaceMock: public ISocketInterface
{
  SocketInterfaceMock() = default;
  MOCK_METHOD(int, bind, 
  (int sockfd, const struct sockaddr *addr, socklen_t addrlen));
  MOCK_METHOD(int, socket, (int domain, int type, int protocol));
  MOCK_METHOD(int, accept, 
  (int sockfd, struct sockaddr *addr, socklen_t *addrlen));
  MOCK_METHOD(int, listen, (int sockfd, int backlog));
  MOCK_METHOD(int, recv, (int sockfd, void *buf, size_t len, int flags));
};

#endif // I_SOCKET_INTERFACE_MOCK_HPP
