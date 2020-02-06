//
// Created by yavorann on 06.02.20.
//

#ifndef EXECUTION_MANAGER_I_SOCKET_HANDLER_HPP
#define EXECUTION_MANAGER_I_SOCKET_HANDLER_HPP

#include <i_socket.hpp>

class ISocketHandler: public ISocket
{
public:
    int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) override;
    int socket(int domain, int type, int protocol) override;
    int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) override;
    int listen(int sockfd, int backlog) override;
    ssize_t recv(int sockfd, char *buf, size_t len, int flags) override;
    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override;
    int poll(struct pollfd *fds, nfds_t nfds, int timeout) override;
    int fcntl(int fd, int cmd, int arg) override;
    int close(int fd) override;
};

#endif //EXECUTION_MANAGER_I_SOCKET_HANDLER_HPP
