#ifndef EXECUTION_MANAGER_I_OS_INTERFACE_HPP
#define EXECUTION_MANAGER_I_OS_INTERFACE_HPP

#include <unistd.h>

class IOsInterface
{
public:
    virtual pid_t fork() = 0;
    virtual int execv(const char* path, char* argv[]) = 0;
    virtual int kill(pid_t procId, int signal) = 0;

    virtual ~IOsInterface() = default;
};

#endif //EXECUTION_MANAGER_I_OS_INTERFACE_HPP