#include "socket_interface.hpp"

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