#include <fcntl.h>
#include <cstdio>
#include "../include/server_socket.hpp"
//#include "server_socket.hpp"

int ServerSocket::socket(int domain, int type, int protocol) const {
    return ::socket(domain, type, protocol);
}

int ServerSocket::close(int fd) const {
    return ::close(fd);
}

int ServerSocket::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) const {
    return ::bind(sockfd, addr, addrlen);
}

int ServerSocket::listen(int sockfd, int backlog) const {
    return ::listen(sockfd, backlog);
}
int ServerSocket::fcntl(int fd, int cmd, int arg) const {
    return ::fcntl(fd, cmd, arg);
}

int ServerSocket::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) const {
    return ::accept(sockfd, addr, addrlen);
}

ssize_t ServerSocket::recv(int sockfd, void *buf, size_t len, int flags) const {
    return ::recv(sockfd, buf, len, flags);
}

ssize_t ServerSocket::send(int sockfd, const void *buf, size_t len, int flags) const {
    return ::send(sockfd, buf, len, flags);
}

int ServerSocket::remove(const char *path) const {
    return std::remove(path);
}