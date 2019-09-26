#include "os_interface.hpp"

#include <unistd.h>
#include <csignal>

namespace ExecutionManager
{

pid_t OsInterface::fork()
{
  return ::fork();
}

int OsInterface::execv(const char* path, char* argv[])
{
  return ::execv(path, argv);
}

int OsInterface::kill(pid_t procId, int signal)
{
  return::kill(procId, signal);
}

}
