#include "os_interface.hpp"

#include <unistd.h>
#include <stdarg.h>
#include <csignal>

namespace ExecutionManager
{

pid_t OsInterface::fork()
{
  return ::fork();
}

int OsInterface::execvp(const char *file, char **argv)
{
    return ::execvp(file, argv);
}

FILE *OsInterface::popen(const char *command, const char *type)
{
    return ::popen(command, type);
}

int OsInterface::pclose(FILE *stream)
{
    return ::pclose(stream);
}

size_t OsInterface::fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    return ::fread(ptr, size, count, stream);
}

}
