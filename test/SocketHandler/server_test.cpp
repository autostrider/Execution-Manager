#include <server.hpp>
#include <server_socket_mock.hpp>
#include <client_mock.hpp>
#include <client_socket_mock.hpp>
#include <constants.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class MyClient
{
public:
    MyClient() {}
    ~MyClient() = default;

    void expectReadDataFromClient(std::string, int);

public:
    std::shared_ptr<ClientMock> clientShPtr =
            std::make_shared<StrictMock<ClientMock>>();
};

void MyClient::expectReadDataFromClient(std::string message, int size)
{
    EXPECT_CALL(*clientShPtr, receive()).WillOnce(Return(message));
    EXPECT_CALL(*clientShPtr, getRecvBytes()).WillRepeatedly(Return(size));
}

class ServerTest : public Test
{
protected:
    ServerTest() {}
    ~ServerTest() noexcept(true) override {}

    void SetUp() override
    {
        ::unlink(SOCKET_SERVER_PATH.c_str());   
    }

    void TearDown() override
    {
        ::unlink(SOCKET_SERVER_PATH.c_str());
    }

    void expectCreateServer();
    void expectAcceptConnection();
    void expectCloseServerSocket();
    
protected:
    const int serverfd = 4;
    const int clientfd = 3;

    std::unique_ptr<ServerSocketMock> socket =
            std::make_unique<StrictMock<ServerSocketMock>>();

    std::unique_ptr<MyClient> myClient = std::make_unique<MyClient>();
};

void ServerTest::expectCreateServer()
{
    EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
            .WillOnce(Return(serverfd));
    EXPECT_CALL(*socket, bind(serverfd, _, sizeof(struct sockaddr_un)))
            .WillOnce(Return(0));
    EXPECT_CALL(*socket, listen(serverfd, _)).WillOnce(Return(1));
}

void ServerTest::expectAcceptConnection()
{
    EXPECT_CALL(*socket, accept(serverfd, _, _)).WillOnce(Return(clientfd));
    EXPECT_CALL(*socket, fcntl(clientfd, _, _)).WillOnce(Return(0));
}

void ServerTest::expectCloseServerSocket()
{
    EXPECT_CALL(*socket, close(serverfd)).WillOnce(Return(0));
}


TEST_F(ServerTest, ShouldSuccessfulyCreateServer)
{
    {
        InSequence sq;
        expectCreateServer();
        expectCloseServerSocket();
    }

    std::unique_ptr<Server> server =
        std::make_unique<Server>(SOCKET_SERVER_PATH, std::move(socket));
}

TEST_F(ServerTest, ShouldSuccessfulyStartServer)
{
    {
        InSequence sq;
        expectCreateServer();
        expectCloseServerSocket();
    }

    std::unique_ptr<Server> server =
        std::make_unique<Server>(SOCKET_SERVER_PATH, std::move(socket));

    server->start();
    EXPECT_EQ(server->isStarted(), true);
}

TEST_F(ServerTest, ShouldSuccessfulyStopServer)
{
    {
        InSequence sq;
        expectCreateServer();
        expectCloseServerSocket();
    }

    std::unique_ptr<Server> server =
        std::make_unique<Server>(SOCKET_SERVER_PATH, std::move(socket));

    server->stop();

    EXPECT_EQ(server->isStarted(), false);

}

TEST_F(ServerTest, ShouldSuccessfulyAcceptConnection)
{
    {
        InSequence sq;
        expectCreateServer();
        expectAcceptConnection();
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(SOCKET_SERVER_PATH, std::move(socket));

    int result = server->acceptConnection();
    EXPECT_EQ(result, clientfd);
}

TEST_F(ServerTest, ShouldSuccessfulyReadFromSocket)
{
    {
        InSequence sq;
        expectCreateServer();
        myClient->expectReadDataFromClient("message", 7);
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(SOCKET_SERVER_PATH, std::move(socket));

    server->readFromSocket(myClient->clientShPtr);
}