#include <proxy_client_factory.hpp>
#include <client_socket_mock.hpp>

#include <type_traits>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

class STest : public Test
{
public:
    ProxyClientFactory factory;
    int fd = 3;
    const std::string socketPath = "/path/to/socket";

    std::unique_ptr<StrictMock<IClientSocketMock>> c_socket =
            std::make_unique<StrictMock<IClientSocketMock>>();
};

TEST_F(STest, Mytest)
{
    {
        EXPECT_CALL(*c_socket, socket(_,_,_)).WillOnce(Return(fd));
        EXPECT_CALL(*c_socket, close(fd)).WillOnce(Return(0));
    }

    std::unique_ptr<api::socket_handler::IClient> client = std::make_unique<Client>(socketPath, std::move(c_socket));
    auto a = factory.makeProxyClient(fd);

    bool result = std::is_same<decltype (client), decltype (a)>::value;
    ASSERT_TRUE(result);
}