#ifndef SOCKET_INTERFACE_HPP
#define SOCKET_INTERFACE_HPP

#include <i_socket_interface.hpp>

namespace MSM
{

class SocketInterface: public ISocketInterface
{
public:
  int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) override;
  int socket(int domain, int type, int protocol) override;
  int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) override;
  int listen(int sockfd, int backlog) override;
  int recv(int sockfd, void *buf, size_t len, int flags) override;
};

}

#endif // SOCKET_INTERFACE_HPP
