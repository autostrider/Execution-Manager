#ifndef CLIENT_CONNECTION_HPP
#define CLIENT_CONNECTION_HPP

#include "i_connection.hpp"

class ClientConnection : public IConnection {
public:

    int socket(int domain, int type, int protocol) const override;

    int connect(int fd, const sockaddr *addr, socklen_t len) const override;

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) const override;

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) const override;

    int close(int fd) const override;
};

#endif //CLIENT_CONNECTION_HPP
