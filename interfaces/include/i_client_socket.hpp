#ifndef I_CLIENT_SOCKET_HPP
#define I_CLIENT_SOCKET_HPP

#include "i_socket.hpp"

class IClientSocket : public ISocket
{
public:
    virtual ~IClientSocket() = default;

    virtual int connect(int fd, const sockaddr *addr, socklen_t len) const = 0;
    virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) const = 0;
    virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags) const = 0;
};

#endif //I_CLIENT_SOCKET_HPP
