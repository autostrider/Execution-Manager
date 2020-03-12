#ifndef I_CLIENT_MOCK_HPP
#define I_CLIENT_MOCK_HPP

#include <i_client.hpp>

class ClientMock : public IClient
{
    ClientMock = default;
    MOCKMETHOD0(void, connect, ());
    MOCKMETHOD1(void, sendMessage, (const google::protobuf::Any&));
    MOCKMETHOD1(std::string, receive, ());
    MOCKMETHOD0(bool, isConnected, ());
};

#endif // I_CLIENT_MOCK_HPP