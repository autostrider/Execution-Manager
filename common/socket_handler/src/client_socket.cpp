#include "client_socket.hpp"

namespace socket_handler
{

int ClientSocket::socket(int domain, int type, int protocol) const
{
    return ::socket(domain, type, protocol);
}

int ClientSocket::connect(int fd, const sockaddr *addr, socklen_t len) const
{
    return ::connect(fd, addr, len);
}

ssize_t ClientSocket::recv(int sockfd, void *buf, size_t len, int flags) const
{
    return ::recv(sockfd, buf, len, flags);
}

ssize_t ClientSocket::send(int sockfd, const void *buf, size_t len, int flags) const
{
    return ::send(sockfd, buf, len, flags);
}

int ClientSocket::close(int fd) const
{
    return ::close(fd);
}

int ClientSocket::shutdown(int fd) const
{
    return ::shutdown(fd, SHUT_RDWR);
}

}