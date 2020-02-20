#ifndef EXECUTION_MANAGER_I_SERVER_SOCKET_HPP
#define EXECUTION_MANAGER_I_SERVER_SOCKET_HPP

#include "i_socket.hpp"


    class IServerSocket : public ISocket {
    public:
        virtual int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) const = 0;

        virtual int listen(int sockfd, int backlog) const = 0;

        virtual int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) const = 0;

        virtual int fcntl(int fd, int cmd, int arg) const = 0;

        virtual int remove(const char *path) const = 0;

        virtual ~IServerSocket() = default;
    };

#endif //EXECUTION_MANAGER_I_SERVER_SOCKET_HPP
