#include <proxy_client_factory.hpp>
#include <client_socket_mock.hpp>

#include <type_traits>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;
using namespace api::client;

class ProxyClientFactoryTest : public Test
{
public:
    ProxyClientFactory factory;
    static constexpr char socketPath[] = "/path/to/socket";
    static constexpr int fd = 13;

    std::unique_ptr<StrictMock<IClientSocketMock>> c_socket =
            std::make_unique<StrictMock<IClientSocketMock>>();
};

constexpr int ProxyClientFactoryTest::fd;
constexpr char ProxyClientFactoryTest::socketPath[];

TEST_F(ProxyClientFactoryTest, MekeClient)
{
    {
        EXPECT_CALL(*c_socket, socket(_,_,_)).WillOnce(Return(fd));
        EXPECT_CALL(*c_socket, close(fd)).WillOnce(Return(0));
    }

    std::unique_ptr<IClient> client = std::make_unique<Client>(socketPath, std::move(c_socket));
    auto makedClient = factory.makeProxyClient(fd);

    bool result = std::is_same<decltype (client), decltype (makedClient)>::value;
    ASSERT_TRUE(result);
}