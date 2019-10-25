#ifndef EXECUTION_MANAGER_I_OS_INTERFACE_HPP
#define EXECUTION_MANAGER_I_OS_INTERFACE_HPP

#include <unistd.h>

class IOsInterface
{
public:
    virtual pid_t fork() = 0;
    virtual int execvp(const char* file, char* argv[]) = 0;

    virtual ~IOsInterface() = default;
};

#endif //EXECUTION_MANAGER_I_OS_INTERFACE_HPP
