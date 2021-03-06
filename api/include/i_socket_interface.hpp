#ifndef I_SOCKET_INTERFACE_HPP
#define I_SOCKET_INTERFACE_HPP

#include <sys/types.h>
#include <sys/socket.h>

namespace api
{

class ISocketInterface
{
public:
    virtual ~ISocketInterface() = default;
    
    virtual int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) = 0;
    virtual int socket(int domain, int type, int protocol) = 0;
    virtual int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) = 0;
    virtual int listen(int sockfd, int backlog) = 0;
    virtual ssize_t recv(int sockfd, char *buf, size_t len, int flags) = 0;
    virtual int close(int fd) = 0 ;
};

}

#endif // I_SOCKET_INTERFACE_HPP
