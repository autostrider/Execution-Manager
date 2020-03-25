#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <i_server_socket.hpp>

using namespace api;

namespace socket_handler
{

class ServerSocket : public IServerSocket
{
public:
    int socket(int domain, int type, int protocol) const override;
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) const override;
    int listen(int sockfd, int backlog) const override;
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) const override;
    int fcntl(int fd, int cmd, int arg) const override;
    int close(int fd) const override;
};

}

#endif //SERVER_SOCKET_HPP
