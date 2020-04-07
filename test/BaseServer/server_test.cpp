#include <server.hpp>
#include <server_socket_mock.hpp>
#include <connection_mock.hpp>
#include <connection_factory_mock.hpp>
#include <constants.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace constants;
using namespace base_server;
using namespace ::testing;


class ServerTest : public Test
{
protected:
    void expectCreateServer();
    void expectCloseServerSocket();
    void expectCreatingAcceptedConnection();
    void expectReadData(std::string, const int);
    
protected:
    const int serverfd = 4;
    const int clientfd = 3;

    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<StrictMock<IServerSocketMock>> socket =
            std::make_unique<StrictMock<IServerSocketMock>>();
    std::shared_ptr<StrictMock<ConnectionMock>> connection =
            std::make_unique<StrictMock<ConnectionMock>>();
    std::unique_ptr<StrictMock<IConnectionFactoryMock>> connFactory =
            std::make_unique<StrictMock<IConnectionFactoryMock>>();

    ConnectionMock* connectionPtr = connection.get();
};

void ServerTest::expectCreateServer()
{
    EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
            .WillOnce(Return(serverfd));
    EXPECT_CALL(*socket, bind(serverfd, _, sizeof(struct sockaddr_un)))
            .WillOnce(Return(0));
    EXPECT_CALL(*socket, listen(serverfd, _)).WillOnce(Return(1));
}

void ServerTest::expectCloseServerSocket()
{
    EXPECT_CALL(*socket, close(serverfd)).WillOnce(Return(0));
}

void ServerTest::expectCreatingAcceptedConnection()
{
    EXPECT_CALL(*connFactory, makeConnection(_, serverfd)).WillRepeatedly(Return(connection));
    EXPECT_CALL(*connectionPtr, creatAcceptedClient()).WillRepeatedly(Return());
}

void ServerTest::expectReadData(std::string message, const int size)
{
    EXPECT_CALL(*connectionPtr, receiveData(_)).WillRepeatedly(Return(size));
}


TEST_F(ServerTest, ShouldSuccessfulyCreateServer)
{
    {
        InSequence sq;
        expectCreateServer();
        expectCloseServerSocket();
    }

    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));
}

TEST_F(ServerTest, ShouldSuccessfulyStopServer)
{
    {
        InSequence sq;
        expectCreateServer();
        expectCloseServerSocket();
    }

    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));
    
    server->stop();

    EXPECT_EQ(server->isStarted(), false);
}

TEST_F(ServerTest, ShouldSuccessfulyCreateConnectionAndAcceptedClient)
{
    {
        expectCreateServer();
        expectCreatingAcceptedConnection();
        expectReadData("message", 7);
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));
    
    server->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(ServerTest, ShouldSuccessfulyReceiveData)
{
    {
        expectCreateServer();
        expectCreatingAcceptedConnection();
        expectReadData("message", 7);
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));
    
    server->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(ServerTest, ShouldSuccessfulyReceiveDataNull)
{
    {
        expectCreateServer();
        expectCreatingAcceptedConnection();
        expectReadData("", 0);
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));
    
    server->start();
    server->readFromSocket(connection);
}

TEST_F(ServerTest, ShouldSuccessfulyGetQueueElement)
{
    std::string str = "hi";
    {
        expectCreateServer();
        expectCreatingAcceptedConnection();
        expectReadData(str, 2);
        expectCloseServerSocket();
    }
    
    std::unique_ptr<Server> server =
        std::make_unique<Server>(socketPath, std::move(socket), std::move(connFactory));

    server->start();

    ASSERT_FALSE(server->getQueueElement(str));
    EXPECT_NE(str, "");
}
