#include <client.hpp>
#include <client_socket_mock.hpp>

#include <thread>
#include <chrono>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class ClientSocketTest: public Test
{
protected:
    const int clientfd = 4;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<ClientSocketMock> c_socket =
            std::make_unique<StrictMock<ClientSocketMock>>();
};


TEST_F(ClientSocketTest, ShouldSuccessfulyCreateAndKillClientSocket)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(socketPath, std::move(c_socket));
}

TEST_F(ClientSocketTest, ShouldSuccessfulyConnect)
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

TEST_F(ClientSocketTest, ShouldSuccessifulySendData)
{
    details dt;
    dt.set_mymessage("some message");

    google::protobuf::Any any;
    any.PackFrom(dt);

    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(clientfd));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, send(clientfd, _, _, 0)).WillOnce(Return(45));
        EXPECT_CALL(*c_socket, close(clientfd)).WillOnce(Return(0));

    }
    std::unique_ptr<Client> client =  std::make_unique<Client>(socketPath, std::move(c_socket));
    client->connect();
    client->sendMessage(any);
}

TEST_F(ClientSocketTest, ShouldSuccessifulyReceiveData)
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

