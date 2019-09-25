#include "os_interface.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <logger.hpp>

namespace ExecutionManager
{

pid_t OsInterface::fork()
{
  return ::fork();
}

int OsInterface::execv(const char* path, char* argv[])
{
  return  ::execv(path, argv);
}

int OsInterface::kill(pid_t procId, int signal)
{
  return::kill(procId, signal);
}

int OsInterface::waitpid(pid_t procId, int *status, int options)
{
  return ::waitpid(procId, status, options);
}

}
