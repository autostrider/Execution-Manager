#include "os_interface.hpp"

#include <unistd.h>
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

}
