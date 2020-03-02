#include "client_connection.hpp"

    int ClientConnection::socket(int domain, int type, int protocol) const {
        return ::socket(domain, type, protocol);
    }

    int ClientConnection::connect(int fd, const sockaddr *addr, socklen_t len) const {
        return ::connect(fd, addr, len);
    }

    ssize_t ClientConnection::recv(int sockfd, void *buf, size_t len, int flags) const {
        return ::recv(sockfd, buf, len, flags);
    }

    ssize_t ClientConnection::send(int sockfd, const void *buf, size_t len, int flags) const {
        return ::send(sockfd, buf, len, flags);
    }

    int ClientConnection::close(int fd) const {
        return ::close(fd);
    }

