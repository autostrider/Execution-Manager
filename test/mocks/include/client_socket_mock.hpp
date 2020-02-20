#ifndef EXECUTION_MANAGER_CLIENT_SOCKET_MOCK_HPP
#define EXECUTION_MANAGER_CLIENT_SOCKET_MOCK_HPP
#include <client_socket.hpp>
#include "gmock/gmock.h"


class ClientSocketMock : public IClientSocket {

public:
    MOCzK_METHOD(int, socket, (int domain, int type, int protocol), (const, override));
    MOCK_METHOD(int, connect, (int fd, const sockaddr *addr, socklen_t len), (const, override));
    MOCK_METHOD(int, close, (int fd), (const, override));
    MOCK_METHOD(ssize_t, recv, (int sockfd, void *buf, size_t len, int flags), (const, override));
    MOCK_METHOD(ssize_t, send, (int sockfd, const void *buf, size_t len, int flags), (const, override));
};

#endif //EXECUTION_MANAGER_CLIENT_SOCKET_MOCK_HPP
