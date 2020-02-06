//
//#include "../mocks/include/i_socket_handler_mock.hpp"
//#include "../../../api/socket_handler/include/i_socket_server.h"
//#include "gtest/gtest.h"
//
//using namespace ::testing;
//
//class SocketServerTests: public Test
//{
//protected:
//    const int socketfd = 1;
//    const std::string socketPath = "/path/to/socket";
//    std::unique_ptr<ISocketHandlerMock> socket = std::make_unique<ISocketHandlerMock>();
//
//};
//
//TEST_F(SocketServerTests, SuccessfulyCreateAndKillServer) {
//    {
//        struct sockaddr_un {};
//        InSequence sq;
//        EXPECT_CALL(*socket, socket(AF_UNIX, SOCK_STREAM, 0))
//                .WillOnce(Return(socketfd));
//        EXPECT_CALL(*socket, bind(socketfd, _, sizeof(sockaddr_un)))
//                .WillOnce(Return(0));
//        EXPECT_CALL(*socket, listen(socketfd, 5)).WillOnce(Return(0));
//        EXPECT_CALL(*socket, close(socketfd));
//    }
//};