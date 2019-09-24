#include "application_handler.hpp"

#include <csignal>
#include <logger.hpp>
#include <exception>
#include <sys/wait.h>

namespace ExecutionManager
{

using std::runtime_error;

ApplicationHandler::ApplicationHandler(std::unique_ptr<IOsInterface> syscalls,
                                       std::string path)
        : corePath{std::move(path)},
          m_syscalls{std::move(syscalls)}
{ }

pid_t ApplicationHandler::startProcess(const ProcessInfo& process)
{
  int code = errno;
  LOG << ":::::::::: BEFORE START PROCESS 1 ERRNO: " << code;
  pid_t processId = m_syscalls->fork();
  int res_code = errno;
  LOG << ":::::::::: AFTER START PROCESS 1 ERRNO: " << res_code;

  if (!processId)
  {
    LOG << processId << " procid fork";
    // child process
    const auto processPath = corePath
                           + process.createRelativePath();

    auto arguments = getArgumentsList(process);
    auto applicationArguments = convertToNullTerminatingArgv(arguments);
    int res = m_syscalls->execv(processPath.c_str(),
                                applicationArguments.data());
    LOG << ":::::::::: START PROCESS 1 ERRNO: " << errno;

    if (res)
    {
      throw runtime_error(std::string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else
  {
    LOG << processId << " procid fork";
  }
  return processId;
}

void ApplicationHandler::killProcess(pid_t processId)
{
  LOG << ":::::::::: kill process 1 ERRNO: " << errno;
  if (!m_syscalls->kill(processId, SIGTERM))
  {
    LOG << ":::::::::: kill process 1 ERRNO: " << errno;
    switch (errno)
    {
      case EINVAL:
        LOG << "----->>> KILL INVALID SIGNAL RECEIVED";
        break;
      case EPERM:
        LOG << "----->>> KILL PROCESS CAN'T SEND SIGNAL TO TARGET PROCESS";
        break;
      case ESRCH:
        LOG << "----->>> KILL PROCESS DOESN'T EXISTS";
        break;
      default:
        LOG << "--->>> KILL UNKNOWN ERROR";
    }
  }

  int status;
  if (!m_syscalls->waitpid(processId, &status, WNOHANG))
  {
    LOG << ":::::::::: kill process 1 ERRNO: " << errno;
    switch (errno)
    {
      case ECHILD:
        LOG << "----->>> WAITPID ERRNO ECHILD";
        break;
      case EINTR:
        LOG << "---->>> ERRNO WAITPID EINTR";
        break;
      default:
        LOG << "---->>> UNKNOWN WAITPID ERROR";
    }
  }
}

std::vector<std::string>
ApplicationHandler::getArgumentsList(const ProcessInfo &process) const
{
  std::vector<std::string> arguments;
  arguments.reserve(process.startOptions.size() + 1);

  // insert app name
  arguments.push_back(process.processName);

  std::transform(process.startOptions.cbegin(),
                 process.startOptions.cend(),
                 std::back_inserter(arguments),
                 [](const StartupOption& option)
                 { return option.makeCommandLineOption(); });

  return arguments;
}

std::vector<char*>
ApplicationHandler::convertToNullTerminatingArgv(
  std::vector<std::string>& vectorToConvert) const
{
  std::vector<char*> outputVector;

  // include terminating sign, that not included in argv
  outputVector.reserve(vectorToConvert.size() + 1);

  for(auto& str: vectorToConvert)
  {
    outputVector.push_back(&str[0]);
  }

  // terminating sign
  outputVector.push_back(nullptr);

  return outputVector;
}

} // namespace ExecutionManager
