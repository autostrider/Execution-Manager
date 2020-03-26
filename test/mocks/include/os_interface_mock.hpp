#ifndef OS_INTERFACE_MOCK_HPP
#define OS_INTERFACE_MOCK_HPP

#include <i_os_interface.hpp>

#include <stdio.h>

#include "gmock/gmock.h"

class OSInterfaceMock : public api::IOsInterface
{
public:
    OSInterfaceMock() = default;

    MOCK_METHOD0(fork, pid_t());
    MOCK_METHOD2(execvp, int(const char* file, char* argv[]));
    MOCK_METHOD2(popen, FILE*(const char* command, const char* type));
    MOCK_METHOD1(pclose, int(FILE *stream));
    MOCK_METHOD4(fread, size_t(void* ptr, size_t size, size_t count, FILE *stream));
};

#endif // OS_INTERFACE_MOCK_HPP
