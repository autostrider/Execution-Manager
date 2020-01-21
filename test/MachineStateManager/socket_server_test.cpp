#include <socket_server.hpp>

#include <mocks/i_socket_interface_mock.hpp>

#include "gtest/gtest.h"

using namespace ::testing;

namespace
{

ACTION_TEMPLATE(SetArgNPointeeTo, HAS_1_TEMPLATE_PARAMS(unsigned, uIndex),
                AND_2_VALUE_PARAMS(pData, uiDataSize))
{
  ::memcpy(std::get<uIndex>(args), pData, uiDataSize);
}

}

class SocketServerTest: public Test
{
protected:
  const int socketfd = 1;
  const std::string socketPath = "/path/to/socket";
  std::unique_ptr<SocketInterfaceMock> socket =
          std::make_unique<StrictMock<SocketInterfaceMock>>();
};

TEST_F(SocketServerTest, ShouldSuccessfulyCreateAndKillServer)
{
  {
    InSequence sq;
    EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
         .WillOnce(Return(socketfd));
    EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
            .WillOnce(Return(0));
    EXPECT_CALL(*socket, listen(socketfd, 5)).WillOnce(Return(0));
    EXPECT_CALL(*socket, close(socketfd));
  }

  MSM::SocketServer ss(std::move(socket), socketPath);
}

TEST_F(SocketServerTest, ShouldSuccessfulyRunServer)
{
  char recvValue[] = "some string";
  size_t recvValSize = sizeof(recvValue);
  constexpr int clientfd = 2;
  {
    InSequence sq;
    EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
            .WillOnce(Return(socketfd));
    EXPECT_CALL(*socket, bind(socketfd, _, sizeof(struct sockaddr_un)))
            .WillOnce(Return(0));
    EXPECT_CALL(*socket, listen(socketfd, 5)).WillOnce(Return(0));
    EXPECT_CALL(*socket, accept(socketfd, _, _)).WillOnce(Return(clientfd));
    EXPECT_CALL(*socket, recv(clientfd, _, _, _))
            .WillRepeatedly(DoAll(
                    SetArgNPointeeTo<1>(std::begin(recvValue), recvValSize),
                    Return(recvValSize)
            ));
    EXPECT_CALL(*socket, close(clientfd));
    EXPECT_CALL(*socket, close(socketfd));
  }

  MSM::SocketServer ss(std::move(socket), socketPath);
  ss.startServer();
  ASSERT_EQ(ss.getData(), recvValue);
  ss.closeServer();
}