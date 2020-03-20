#include <connection.hpp>
#include <server_socket_mock.hpp>
#include <proxy_client_factory_mock.hpp>
#include <client_mock.hpp>
#include <connection_factory.hpp>

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class ConnectionTest : public Test
{
protected:
    const int serverFd = 1;
    const int clientFd = 2;
    
    std::shared_ptr<StrictMock<IServerSocketMock>> socket = std::make_unique<StrictMock<IServerSocketMock>>();
    std::unique_ptr<StrictMock<ClientMock>> client = std::make_unique<StrictMock<ClientMock>>();
    std::unique_ptr<StrictMock<ProxyClientFactoryMock>> proxyClientFactory = std::make_unique<StrictMock<ProxyClientFactoryMock>>();

    ClientMock* clientPtr = client.get();
    ProxyClientFactoryMock* proxyClientFactoryPtr = proxyClientFactory.get();
};

TEST_F(ConnectionTest, ShouldSuccessfulyAcceptConnection)
{
    ConnectionFactory connectionFactory (std::move(proxyClientFactory));
    auto connection = connectionFactory.makeConnection(socket, serverFd);
    {
        InSequence sq;
        EXPECT_CALL(*socket, accept(serverFd, _, _)).WillOnce(Return(clientFd));
        EXPECT_CALL(*socket, fcntl(clientFd, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*proxyClientFactoryPtr, makeProxyClient(clientFd)).WillOnce(Return(ByMove(std::move(client))));
    }
    connection->creatAcceptedClient();
}

TEST_F(ConnectionTest, ShouldNotAcceptConnection)
{
    ConnectionFactory connectionFactory (std::move(proxyClientFactory));
    auto connection = connectionFactory.makeConnection(socket, serverFd);
    int errorFd = -1;
    {
        InSequence sq;
        EXPECT_CALL(*socket, accept(serverFd, _, _)).WillOnce(Return(errorFd));
        EXPECT_CALL(*socket, fcntl(errorFd, _, _)).WillOnce(Return(-1));
    }
    connection->creatAcceptedClient();
}

TEST_F(ConnectionTest, ShouldSuccessfulyReceiveData)
{
    ConnectionFactory connectionFactory (std::move(proxyClientFactory));
    auto connection = connectionFactory.makeConnection(socket, serverFd);
    std::string message = "hi";
    {
        InSequence sq;
        EXPECT_CALL(*socket, accept(serverFd, _, _)).WillOnce(Return(clientFd));
        EXPECT_CALL(*socket, fcntl(clientFd, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*proxyClientFactoryPtr, makeProxyClient(clientFd)).WillOnce(Return(ByMove(std::move(client))));
        EXPECT_CALL(*clientPtr, receive()).WillOnce(Return(message));
        EXPECT_CALL(*clientPtr, getRecvBytes()).WillOnce(Return(2));
    }
    connection->creatAcceptedClient();
    std::string result = connection->receiveData();
    int res = connection->getRecvBytes();

    EXPECT_EQ(result, message);
    EXPECT_EQ(res, 2);
}

