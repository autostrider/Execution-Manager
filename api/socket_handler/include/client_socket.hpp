#ifndef EXECUTION_MANAGER_CLIENT_SOCKET_HPP
#define EXECUTION_MANAGER_CLIENT_SOCKET_HPP

#include "i_socket.hpp"
#include "i_client_socket.hpp"
#include <zconf.h>


class ClientSocket : public  IClientSocket {
public:

    int socket(int domain, int type, int protocol) const override;

    int connect(int fd, const sockaddr *addr, socklen_t len) const override;

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) const override;

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) const override;

    int close(int fd) const override;
};

#endif //EXECUTION_MANAGER_CLIENT_SOCKET_HPP
