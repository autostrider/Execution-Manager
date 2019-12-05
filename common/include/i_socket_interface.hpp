#ifndef I_SOCKET_INTERFACE_HPP
#define I_SOCKET_INTERFACE_HPP

#include <sys/types.h>
#include <sys/socket.h>

class ISocketInterface
{
public:
  virtual int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) = 0;
  virtual int socket(int domain, int type, int protocol) = 0;
  virtual int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) = 0;
  virtual int listen(int sockfd, int backlog) = 0;
  virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) = 0;

  virtual ~ISocketInterface() = default;
};

#endif // I_SOCKET_INTERFACE_HPP
