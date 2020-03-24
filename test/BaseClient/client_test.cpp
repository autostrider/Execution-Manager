#include <client.hpp>
#include <client_socket_mock.hpp>
#include <constants.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;
using namespace api::client;
using namespace api::socket_handler;


class ClientTest : public Test
{
protected:
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<StrictMock<IClientSocketMock>> c_socket =
            std::make_unique<StrictMock<IClientSocketMock>>();
};

TEST_F(ClientTest, ShouldSuccessfulyCreateClientSocket)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(socketPath, std::move(c_socket));
}

TEST_F(ClientTest, ShouldSuccessfulyConnectToServer)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(socketPath, std::move(c_socket));
    client->connect();

    ASSERT_EQ(client->isConnected(), true);
}

TEST_F(ClientTest, ShouldNotConnectToServer)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(-1));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(socketPath, std::move(c_socket));
    client->connect();

    ASSERT_EQ(client->isConnected(), false);
}

TEST_F(ClientTest, ShouldSuccessifulySendData)
{
    std::string message = "some message";
    details dt;
    dt.set_mymessage(message);

    google::protobuf::Any any;
    any.PackFrom(dt);
    
    size_t size = any.ByteSizeLong();

    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, send(clientfd, _, _, 0)).WillOnce(Return(size));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));

    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    client->connect();
    client->sendMessage(any);
}

TEST_F(ClientTest, ShouldSuccessifulyReceiveData)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, recv(clientfd, _, _,_)).WillOnce(Return(13));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    client->receive();

    EXPECT_EQ(client->getRecvBytes(), 13);
}

TEST_F(ClientTest, ShouldNotReceiveData)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, recv(clientfd, _, _,_)).WillOnce(Return(-1));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    client->receive();

    EXPECT_EQ(client->getRecvBytes(), -1);
}

TEST_F(ClientTest, ShouldSuccessifulyGetClientFd)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    int resualt = client->getClientFd();

    EXPECT_EQ(resualt, clientfd);
}

TEST_F(ClientTest, ShouldSuccessifulySetClientFd)
{
    int setFd = 13;
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(setFd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    client->setClientFd(setFd);
    int resualt = client->getClientFd();

    EXPECT_EQ(resualt, 13);
}

TEST_F(ClientTest, ShouldSuccessifulySetConnected)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));

    client->setConnected(true);
    bool resualt = client->isConnected();

    EXPECT_EQ(resualt, true);
}