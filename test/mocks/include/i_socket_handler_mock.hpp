//
// Created by yavorann on 04.02.20.
//

#ifndef EXECUTION_MANAGER_I_SOCKET_HANDLER_MOCK_HPP
#define EXECUTION_MANAGER_I_SOCKET_HANDLER_MOCK_HPP

#include <gmock/gmock.h>
#include "../../../api/socket_handler/include/i_socket_handler.hpp"
//#include "../../api/socket_handler/src/socket_server.cpp"

class ISocketHandlerMock : public ISocketHandler

        {
public:
    ISocketHandlerMock() = default;
    MOCK_METHOD3(bind, int(int sockfd, const struct sockaddr *addr, socklen_t addrlen));
    MOCK_METHOD3(socket, int(int domain, int type, int protocol));
    MOCK_METHOD3(accept, int(int sockfd, struct sockaddr *addr, socklen_t *addrlen));
    MOCK_METHOD2(listen, int(int sockfd, int backlog));
    MOCK_METHOD4(recv, ssize_t(int sockfd, char *buf, size_t len, int flags));
    MOCK_METHOD4(send, ssize_t(int sockfd, char *buf, size_t len, int flags));
    MOCK_METHOD1(close, int(int fd));
};

#endif //EXECUTION_MANAGER_I_SOCKET_HANDLER_MOCK_HPP
