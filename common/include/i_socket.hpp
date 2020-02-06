//
// Created by yavorann on 06.02.20.
//

#ifndef EXECUTION_MANAGER_I_SOCKET_HPP
#define EXECUTION_MANAGER_I_SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

class ISocket
{
public:
    virtual int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) = 0;
    virtual int socket(int domain, int type, int protocol) = 0;
    virtual int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) = 0;
    virtual int listen(int sockfd, int backlog) = 0;
    virtual ssize_t recv(int sockfd, char *buf, size_t len, int flags) = 0;
    virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags)  = 0;
    virtual int close(int fd) = 0 ;
    virtual int poll(struct pollfd *fds, nfds_t nfds, int timeout) = 0;
    virtual int fcntl(int fd, int cmd, int arg) = 0;

    virtual ~ISocket() = default;

};

#endif //EXECUTION_MANAGER_I_SOCKET_HPP
