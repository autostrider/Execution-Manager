#ifndef SERVER_SOCKET_MOCK_HPP
#define SERVER_SOCKET_MOCK_HPP

#include <i_server_socket.hpp>

#include "gmock/gmock.h"

using namespace api;

class IServerSocketMock : public IServerSocket
{
public:
    MOCK_METHOD(int, socket, (int domain, int type, int protocol), (const, override));
    MOCK_METHOD(int, close, (int fd), (const, override));
    MOCK_METHOD(int, fcntl, (int fd, int cmd, int arg), (const, override));
    MOCK_METHOD(int, bind, (int sockfd, const struct sockaddr *addr, socklen_t addrlen), (const, override));
    MOCK_METHOD(int, listen, (int sockfd, int backlog), (const, override));
    MOCK_METHOD(int, accept, (int sockfd, struct sockaddr *addr, socklen_t *addrlen), (const, override));
    MOCK_METHOD(int, shutdown, (int socket_fd), (const, override));
};

#endif //SERVER_SOCKET_MOCK_HPP
