#ifndef EXECUTION_MANAGER_I_CLIENT_SOCKET_HPP
#define EXECUTION_MANAGER_I_CLIENT_SOCKET_HPP

#include <i_socket.hpp>
#include <sys/socket.h>

class IClientSocket : public ISocket {
public:
    virtual int socket(int domain, int type, int protocol) const = 0;

    virtual int close(int socket_fd) const = 0;

    virtual int connect(int fd, const sockaddr *addr, socklen_t len) const = 0;

    virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) const = 0;

    virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags) const = 0;

    virtual ~IClientSocket() = default;
};

#endif //EXECUTION_MANAGER_I_CLIENT_SOCKET_HPP
