#ifndef I_SOCKET_INTERFACE_MOCK_HPP
#define I_SOCKET_INTERFACE_MOCK_HPP

#include <i_socket_interface.hpp>

#include "gmock/gmock.h"

class SocketInterfaceMock: public ISocketInterface
{
public:
  SocketInterfaceMock() = default;
  MOCK_METHOD3(bind, 
  int(int sockfd, const struct sockaddr *addr, socklen_t addrlen));
  MOCK_METHOD3(socket, int(int domain, int type, int protocol));
  MOCK_METHOD3(accept, int(int sockfd, struct sockaddr *addr, socklen_t *addrlen));
  MOCK_METHOD2(listen, int(int sockfd, int backlog));
  MOCK_METHOD4(recv, ssize_t(int sockfd, char *buf, size_t len, int flags));
  MOCK_METHOD1(close, int(int fd));
};

#endif // I_SOCKET_INTERFACE_MOCK_HPP
