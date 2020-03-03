#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fcntl.h>
#include "mocks/server_socket_mock.hpp"
#include "mocks/client_socket_mock.hpp"
#include <server.hpp>
#include <thread>
#include <chrono>


using namespace ::testing;

namespace
{

    ACTION_TEMPLATE(SetArgNPointeeTo, HAS_1_TEMPLATE_PARAMS(unsigned, uIndex),
                    AND_2_VALUE_PARAMS(pData, uiDataSize))
    {
        ::memcpy(std::get<uIndex>(args), pData, uiDataSize);
    }

}

class SocketServerTest: public Test
{
protected:
    const int socketfd = 3;
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<ServerSocketMock> socket =
            std::make_unique<StrictMock<ServerSocketMock>>();
    std::unique_ptr<ClientSocketMock> c_socket =
            std::make_unique<StrictMock<ClientSocketMock>>();

};


TEST_F(SocketServerTest, ShouldSuccessfulyCreateAndKillServer)
{
    {
        InSequence sq;
        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
                .WillOnce(Return(socketfd));
        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
                .WillOnce(Return(0));
        EXPECT_CALL(*socket, listen(socketfd, _)).WillOnce(Return(1));
        EXPECT_CALL(*socket, close(socketfd)).WillOnce(Return(0));
        EXPECT_CALL(*socket, remove)
                .Times(1)
                .WillOnce(Return(0));
    }
    std::unique_ptr<Server> s = std::make_unique<Server> (socketPath, std::move(socket));
}


TEST_F(SocketServerTest, ShouldSuccessfulyAcceptConnection)
{
    {
        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
                .WillOnce(Return(socketfd));
        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
                .WillOnce(Return(1));
        EXPECT_CALL(*socket, listen(socketfd, _)).WillOnce(Return(1));

        EXPECT_CALL(*socket, accept(socketfd, _, _)).WillRepeatedly(Return(clientfd));

        EXPECT_CALL(*socket, fcntl(clientfd, _, O_NONBLOCK)).WillRepeatedly(Return(0));

        EXPECT_CALL(*socket, close(socketfd)).WillOnce(Return(0));

        EXPECT_CALL(*socket, remove)
                .Times(1)
                .WillOnce(Return(0));
    }

    std::unique_ptr<Server> s = std::make_unique<Server> (socketPath, std::move(socket));
    s->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    s->stop();
}


TEST_F(SocketServerTest, ShouldSuccessfulyReadData)
{
    char recvValue[] = "some string";
    std::string message = "some string";
    size_t recvValSize = sizeof(recvValue);
    {
        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
                .WillOnce(Return(socketfd));
        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
                .WillOnce(Return(1));
        EXPECT_CALL(*socket, listen(socketfd, _)).WillOnce(Return(1));

        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(1));

        EXPECT_CALL(*socket, accept(socketfd, _, _)).WillRepeatedly(Return(clientfd));
        EXPECT_CALL(*socket, fcntl(clientfd, _, O_NONBLOCK)).WillRepeatedly(Return(0));

        EXPECT_CALL(*c_socket, recv(clientfd, _, _, _))
                .WillRepeatedly(DoAll(
                        SetArgNPointeeTo<1>(std::begin(recvValue), recvValSize),
                        Return(recvValSize)
                ));
        EXPECT_CALL(*c_socket, send(clientfd, _, _, MSG_NOSIGNAL)).WillOnce(Return(1));

        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*socket, close(socketfd)).WillOnce(Return(0));

        EXPECT_CALL(*socket, remove)
                .Times(1)
                .WillOnce(Return(0));
    }

    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(socket));
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    server->start();
    client->connect();

    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto result  = client->receive(message);
    client->sendBytes(message);
    ASSERT_EQ(result, recvValSize);

    server->stop();
}