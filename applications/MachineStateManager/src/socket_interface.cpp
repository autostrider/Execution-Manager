#include "socket_interface.hpp"

namespace MSM
{

int 
SocketInterface::bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{ 
  return ::bind(sockfd, addr, addrlen);
}

int  SocketInterface::socket(int domain, int type, int protocol) 
{ 
  return ::socket(domain, type, protocol);
}

int 
SocketInterface::accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) 
{
  return ::accept(sockfd, addr, addrlen);
}

int SocketInterface::listen(int sockfd, int backlog) 
{
  return ::listen(sockfd, backlog);
}

ssize_t SocketInterface::recv(int sockfd, void *buf, size_t len, int flags)
{
  return ::recv(sockfd, buf, len, flags);
}

}
