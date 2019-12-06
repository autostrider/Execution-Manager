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
  ssize_t recv(int sockfd, char *buf, size_t len, int flags) override;
  int close(int fd) override;
};

}

#endif // SOCKET_INTERFACE_HPP
