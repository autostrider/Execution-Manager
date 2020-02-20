#ifndef EXECUTION_MANAGER_I_SERVER_MOCK_HPP
#define EXECUTION_MANAGER_I_SERVER_MOCK_HPP


#include <i_server.hpp>
#include "gmock/gmock.h"

class ServerMock : public IServer {
public:
    ServerMock() = default;

    MOCK_METHOD0(bool, start, ());
    MOCK_METHOD0(void, stop, ());
};

#endif //EXECUTION_MANAGER_I_SERVER_MOCK_HPP
