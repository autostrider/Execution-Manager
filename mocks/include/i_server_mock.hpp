#ifndef I_SERVER_MOCK_HPP
#define I_SERVER_MOCK_HPP

#include <i_server.hpp>
#include "gmock/gmock.h"

class ServerMock : public IServer {
public:
    ServerMock() = default;
    MOCK_METHOD0(bool, start, ());
    MOCK_METHOD0(void, stop, ());
};

#endif //I_SERVER_MOCK_HPP
