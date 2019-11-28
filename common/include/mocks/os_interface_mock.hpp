#ifndef OS_INTERFACE_MOCK_HPP
#define OS_INTERFACE_MOCK_HPP

#include "i_os_interface.hpp"
#include <stdio.h>
#include "gmock/gmock.h"

class OSInterfaceMock : public IOsInterface
{
public:
    OSInterfaceMock(){};

    MOCK_METHOD(pid_t, fork, ());
    MOCK_METHOD(int, execvp, (const char* file, char* argv[]));
    MOCK_METHOD(FILE*, popen, (const char* command, const char* type));
    MOCK_METHOD(int, pclose, (FILE *stream));
    MOCK_METHOD(size_t, fread,(void* ptr, size_t size, size_t count, FILE *stream));
};

#endif // OS_INTERFACE_MOCK_HPP
