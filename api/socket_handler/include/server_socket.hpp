#ifndef EXECUTION_MANAGER_SERVER_SOCKET_HPP
#define EXECUTION_MANAGER_SERVER_SOCKET_HPP

#include "i_server_socket.hpp"


    class ServerSocket : public IServerSocket {
    public:
        int socket(int domain, int type, int protocol) const override;

        int fcntl(int fd, int cmd, int arg) const override;

        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) const override;

        int listen(int sockfd, int backlog) const override;

        int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) const override;

        int close(int fd) const override;

        int remove(const char *path) const override;
    };

#endif //EXECUTION_MANAGER_SERVER_SOCKET_HPP
