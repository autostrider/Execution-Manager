//
// Created by yavorann on 06.02.20.
//

#include "../include/i_socket_handler.hpp"


int ISocketHandler::bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
    return ::bind(sockfd, addr, addrlen);
}

int  ISocketHandler::socket(int domain, int type, int protocol)
{
    return ::socket(domain, type, protocol);
}

int ISocketHandler::accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
    return ::accept(sockfd, addr, addrlen);
}

int ISocketHandler::listen(int sockfd, int backlog)
{
    return ::listen(sockfd, backlog);
}

ssize_t ISocketHandler::recv(int sockfd, char *buf, size_t len, int flags)
{
    return ::recv(sockfd, buf, len, flags);
}

ssize_t ISocketHandler::send(int sockfd, const void *buf, size_t len, int flags) {
    return ::send(sockfd, buf, len, flags);
}

int ISocketHandler::poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    return ::poll(fds, nfds, timeout);
}

int ISocketHandler::fcntl(int fd, int cmd, int arg) {
    return ::fcntl(fd, cmd, arg);
}

int ISocketHandler::close(int fd)
{
    return ::close(fd);
}
