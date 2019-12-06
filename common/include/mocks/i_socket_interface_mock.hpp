#ifndef I_SOCKET_INTERFACE_MOCK_HPP
#define I_SOCKET_INTERFACE_MOCK_HPP

#include "i_socket_interface.hpp"

#include "gmock/gmock.h"

class SocketInterfaceMock: public ISocketInterface
{
public:
  SocketInterfaceMock() = default;
  MOCK_METHOD(int, bind, 
  (int sockfd, const struct sockaddr *addr, socklen_t addrlen));
  MOCK_METHOD(int, socket, (int domain, int type, int protocol));
  MOCK_METHOD(int, accept, 
  (int sockfd, struct sockaddr *addr, socklen_t *addrlen));
  MOCK_METHOD(int, listen, (int sockfd, int backlog));
  MOCK_METHOD(ssize_t, recv, (int sockfd, char *buf, size_t len, int flags));
  MOCK_METHOD(int, close, (int fd));
};

#endif // I_SOCKET_INTERFACE_MOCK_HPP
