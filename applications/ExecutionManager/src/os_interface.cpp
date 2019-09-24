#include "os_interface.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <logger.hpp>

namespace ExecutionManager
{

pid_t OsInterface::fork()
{
  int code = errno;
  LOG << "BEFORE OSINTERFACE FORK::: " << code;
  int res = ::fork();
  code = errno;
  LOG << "AFTER OSINTERFACE FORK::: " << code;
  return res;
}

int OsInterface::execv(const char* path, char* argv[])
{
  int code = errno;
  LOG << "BEFORE OSINTERFACE EXECV::: " << code;
  int res = ::execv(path, argv);
  code = errno;
  LOG << "AFTER OSINTERFACE EXECV::: " << code;
  return res;
}

int OsInterface::kill(pid_t procId, int signal)
{
  int code = errno;
  LOG << "BEFORE OSINTERFACE KILL::: " << code;
  int res = ::kill(procId, signal);
  code = errno;
  LOG << "AFTER OSINTERFACE KILL::: " << code;
  return res;
}

int OsInterface::waitpid(pid_t procId, int *status, int options)
{
  int code = errno;
  LOG << "BEFORE OSINTERFACE waitpid::: " << code;
  int res = ::waitpid(procId, status, options);
  code = errno;
  LOG << "AFTER OSINTERFACE waitpid::: " << code;
  return res;
}

}
