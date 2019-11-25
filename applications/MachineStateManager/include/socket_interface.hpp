#ifndef SOCKET_INTERFACE_HPP
#define SOCKET_INTERFACE_HPP

#include <i_socket_interface.hpp>

class SocketInterface: public ISocketInterface
{
  int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) override;
  int socket(int domain, int type, int protocol) override;
  int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) override;
  int listen(int sockfd, int backlog) override;
};

#endif // SOCKET_INTERFACE_HPP
