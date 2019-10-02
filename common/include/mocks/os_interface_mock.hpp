#ifndef OSINTERFACEMOCK_HPP
#define OSINTERFACEMOCK_HPP

#include "i_os_interface.hpp"

#include "gmock/gmock.h"

class OSInterfaceMock : public IOsInterface
{
public:
    OSInterfaceMock(){};

    MOCK_METHOD(pid_t, fork, ());
    MOCK_METHOD(int, execv, (const char* path, char* argv[]));
    MOCK_METHOD(int, kill, (pid_t procId, int signal));
};

#endif // OSINTERFACEMOCK_HPP