#include <client.hpp>
#include <client_socket_mock.hpp>
#include <constants.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class ClientTest : public Test
{
protected:
    ClientTest() {}
    ~ClientTest() noexcept(true) override {}

    void SetUp() override
    {
        ::unlink(SOCKET_CLIENT_PATH.c_str());
    }

    void TearDown() override
    {
        ::unlink(SOCKET_CLIENT_PATH.c_str());
    }

    const int clientfd = 4;

    std::unique_ptr<ClientSocketMock> c_socket =
            std::make_unique<StrictMock<ClientSocketMock>>();
};

TEST_F(ClientTest, ShouldSuccessfulyCreateClientSocket)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(SOCKET_CLIENT_PATH, std::move(c_socket));
}

TEST_F(ClientTest, ShouldSuccessfulyConnecToServer)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, connect(_, _,_)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }
    std::unique_ptr<Client> client = std::make_unique<Client>(SOCKET_CLIENT_PATH, std::move(c_socket));
    client->connect();

    ASSERT_EQ(client->isConnected(), true);
}

TEST_F(ClientTest, ShouldSuccessifulySendData)
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
    std::unique_ptr<Client> client =  std::make_unique<Client>(SOCKET_CLIENT_PATH, std::move(c_socket));
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
    std::unique_ptr<Client> client =  std::make_unique<Client>(SOCKET_CLIENT_PATH, std::move(c_socket));

    client->receive();

    EXPECT_EQ(client->getRecvBytes(), 13);
}

