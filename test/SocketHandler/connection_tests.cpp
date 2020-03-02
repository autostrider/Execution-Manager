#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/server_socket_mock.hpp"
#include "mocks/connection_mock.hpp"
#include <server.hpp>
#include <thread>
#include <chrono>


using namespace ::testing;

class ConnectionTest: public Test
{
protected:
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<ServerSocketMock> s_socket =
            std::make_unique<NiceMock<ServerSocketMock>>();
    std::unique_ptr<ConnectionMock> c_socket =
            std::make_unique<StrictMock<ConnectionMock>>();
};


TEST_F(ConnectionTest, ShouldSuccessfulyCreateAndKillConnection)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));
    client->createSocket();
}


TEST_F(ConnectionTest, getConnectionFd)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(4)).WillOnce(Return(0));
    }
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));
    client->createSocket();
    ASSERT_EQ(client->getConnectionFd(), 4);
}


TEST_F(ConnectionTest, setClientFd)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(5)).WillOnce(Return(0));
    }
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));
    client->createSocket();
    client->setConnectionFd(5);
    ASSERT_EQ(client->getConnectionFd(), 5);
}


TEST_F(ConnectionTest, IsConnectedSuccessfulyConnectToServer)
{
    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));

    }
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));
    ASSERT_EQ(client->isConnected(), false);

    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    server->start();
    client->createSocket();
    client ->connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_EQ(client->isConnected(), true);
    server->stop();
}


TEST_F(ConnectionTest, ShouldSuccessifulySendData)
{
    std::string message = "some string";

    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, send(clientfd, _, _, MSG_NOSIGNAL)).WillOnce(Return(1));

    }
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));
    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    server->start();
    client->createSocket();
    client ->connect();
    client->sendBytes(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    server->stop();
}

TEST_F(ConnectionTest, ShouldSuccessifulyReceiveData)
{
    std::string message = "some string";

    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));

        EXPECT_CALL(*c_socket, recv(clientfd, _, _,_)).WillOnce(Return(1));

    }
    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    std::unique_ptr<Connection> client =  std::make_unique<Connection>(socketPath, std::move(c_socket));

    server->start();
    client->createSocket();
    client ->connect();
    client->receive(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    server->stop();
}
