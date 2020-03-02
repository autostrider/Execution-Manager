#ifndef EXECUTION_MANAGER_I_CONNECTION_HPP
#define EXECUTION_MANAGER_I_CONNECTION_HPP

#include "i_socket.hpp"

class IConnection : public ISocket
{
public:
    virtual ~IConnection() = default;
    
    virtual int connect(int fd, const sockaddr *addr, socklen_t len) const = 0;
    virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) const = 0;
    virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags) const = 0;
};

#endif //EXECUTION_MANAGER_I_CONNECTION_HPP
