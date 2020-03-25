#ifndef EXECUTION_MANAGER_I_OS_INTERFACE_HPP
#define EXECUTION_MANAGER_I_OS_INTERFACE_HPP

#include <unistd.h>
#include <stdio.h>

namespace api {

class IOsInterface
{
public:
    virtual ~IOsInterface() = default;
    
    virtual pid_t fork() = 0;
    virtual int execvp(const char* file, char* argv[]) = 0;
    virtual FILE *popen(const char* command, const char* type) = 0;
    virtual int pclose(FILE *stream) = 0;
    virtual size_t fread(void* ptr, size_t size, size_t count, FILE *stream) = 0;
};

}

#endif //EXECUTION_MANAGER_I_OS_INTERFACE_HPP
