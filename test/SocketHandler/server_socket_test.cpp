#include <server_socket_mock.hpp>
#include <server.hpp>

#include <chrono>
#include <fcntl.h>
#include <thread>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class SocketServerTest: public Test
{
protected:
    const int socketfd = 3;
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<ServerSocketMock> socket =
            std::make_unique<StrictMock<ServerSocketMock>>();
};


TEST_F(SocketServerTest, ShouldSuccessfulyCreateAndKillServerSocket)
{
    {
        InSequence sq;
        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
                .WillOnce(Return(socketfd));
        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
                .WillOnce(Return(0));
        EXPECT_CALL(*socket, listen(socketfd, _)).WillOnce(Return(1));
        EXPECT_CALL(*socket, close(socketfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Server> s = std::make_unique<Server> (socketPath, std::move(socket));
}


TEST_F(SocketServerTest, ShouldSuccessfulyAcceptConnection)
{
    {
        InSequence sq;
        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
                .WillOnce(Return(socketfd));
        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
                .WillOnce(Return(1));
        EXPECT_CALL(*socket, listen(socketfd, _)).WillOnce(Return(1));
    
        EXPECT_CALL(*socket, accept(socketfd, _, _)).WillOnce(Return(clientfd));
        EXPECT_CALL(*socket, fcntl(clientfd, _, _)).WillOnce(Return(0));

        EXPECT_CALL(*socket, close(socketfd)).WillOnce(Return(0));
    }

    std::unique_ptr<Server> s = std::make_unique<Server> (socketPath, std::move(socket));

    int resualt = s->acceptConnection();

    EXPECT_EQ(resualt, clientfd);
}
