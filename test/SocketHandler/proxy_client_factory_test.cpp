#include <proxy_client_factory.hpp>
#include <client.hpp>
#include <client_socket_mock.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class ProxyClientTest : public Test
{
protected:
    const int fd = 1;
    
    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<StrictMock<IClientSocketMock>> c_socket =
            std::make_unique<StrictMock<IClientSocketMock>>();

    std::unique_ptr<api::socket_handler::IProxyClientFactory> proxyClientFactory = std::make_unique<ProxyClientFactory>();
};

TEST_F(ProxyClientTest, ShouldSuccesfullyCreatedProxyClient)
{
    {
        InSequence sq;
        EXPECT_CALL(*c_socket, socket(AF_UNIX, SOCK_STREAM, 0)).WillOnce(Return(0));
        EXPECT_CALL(*c_socket, close(0)).WillOnce(Return(0));
    }

    std::unique_ptr<Client> client = std::make_unique<Client>(socketPath, std::move(c_socket));
    proxyClientFactory->makeProxyClient(fd);

    EXPECT_EQ(client->getClientFd(), fd);
    EXPECT_EQ(client->isConnected(), true);
}