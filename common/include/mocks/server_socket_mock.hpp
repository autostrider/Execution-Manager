#ifndef EXECUTION_MANAGER_SERVER_SOCKET_MOCK_HPP
#define EXECUTION_MANAGER_SERVER_SOCKET_MOCK_HPP

#include "gmock/gmock.h"
#include "server_socket.hpp"

class ServerSocketMock : public IServerSocket {
public:
    MOCK_METHOD(int, socket, (int domain, int type, int protocol), (const, override));
    MOCK_METHOD(int, close, (int fd), (const, override));
    MOCK_METHOD(int, fcntl, (int fd, int cmd, int arg), (const, override));
    MOCK_METHOD(int, bind, (int sockfd, const struct sockaddr *addr, socklen_t addrlen), (const, override));
    MOCK_METHOD(int, listen, (int sockfd, int backlog), (const, override));
    MOCK_METHOD(int, remove, (const char *path), (const, override));
    MOCK_METHOD(int, accept, (int sockfd, struct sockaddr *addr, socklen_t *addrlen), (const, override));
};

#endif //EXECUTION_MANAGER_SERVER_SOCKET_MOCK_HPP
