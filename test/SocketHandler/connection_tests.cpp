#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/server_socket_mock.hpp"
#include "mocks/client_socket_mock.hpp"
#include <server.hpp>
#include <thread>
#include <chrono>

using namespace ::testing;

class ClientSocketTest: public Test
{
protected:
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<ServerSocketMock> s_socket =
            std::make_unique<NiceMock<ServerSocketMock>>();
    std::unique_ptr<ClientSocketMock> c_socket =
            std::make_unique<StrictMock<ClientSocketMock>>();
};


TEST_F(ClientSocketTest, ShouldSuccessfulyCreateAndKillConnection)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    client->createSocket();
}


TEST_F(ClientSocketTest, getConnectionFd)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(4)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    client->createSocket();
    ASSERT_EQ(client->getClientFd(), 4);
}


TEST_F(ClientSocketTest, setClientFd)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(5)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    client->createSocket();
    client->setClientFd(5);
    ASSERT_EQ(client->getClientFd(), 5);
}


TEST_F(ClientSocketTest, IsConnectedSuccessfulyConnectToServer)
{
    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));

        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));

    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    ASSERT_EQ(client->isConnected(), false);

    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    server->start();
    client->createSocket();
    client ->connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_EQ(client->isConnected(), true);
    server->stop();
}


TEST_F(ClientSocketTest, ShouldSuccessifulySendData)
{
    std::string message = "some string";

    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, send(clientfd, _, _, MSG_NOSIGNAL)).WillOnce(Return(1));

    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    server->start();
    client->createSocket();
    client ->connect();
    client->sendBytes(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    server->stop();
}

TEST_F(ClientSocketTest, ShouldSuccessifulyReceiveData)
{
    std::string message = "some string";

    {
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));

        EXPECT_CALL(*c_socket, recv(clientfd, _, _,_)).WillOnce(Return(1));

    }
    std::unique_ptr<Server> server = std::make_unique<Server> (socketPath, std::move(s_socket));
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    server->start();
    client->createSocket();
    client ->connect();
    client->receive(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    server->stop();
}
