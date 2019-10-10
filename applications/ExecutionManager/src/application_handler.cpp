#include "application_handler.hpp"

#include <csignal>
#include <exception>
#include <logger.hpp>

namespace ExecutionManager
{

using std::runtime_error;

ApplicationHandler::ApplicationHandler(std::unique_ptr<IOsInterface> syscalls,
                                       std::string path)
        : corePath{std::move(path)},
          m_syscalls{std::move(syscalls)}
{ }

pid_t ApplicationHandler::startProcess(const std::string &process)
{
  return execProcess(process, SystemCtlAction::START);
}

void ApplicationHandler::killProcess(const std::string &process)
{
  execProcess(process, SystemCtlAction::STOP);
}

int
ApplicationHandler::execProcess(const std::string &processName,
                                SystemCtlAction action) const
{
  pid_t process = m_syscalls->fork();

  if (!process)
  {
    std::vector<std::string> arguments =
    {
      systemCtl,
      systemctlActions.at(action),
      processName + SERVICE_EXTENSION
    };
    for (const auto& arg: arguments)
    {
      LOG << arg;
    }
//     auto applicationArgs = convertToNullTerminatingArgv(arguments);
//     int res = m_syscalls->execv(systemCtl.c_str(),
//                                 applicationArgs.data());
// 
//     if (res)
//     {
//       throw runtime_error(std::string{"Error occured creating process: "}
//                           + processName
//                           + " "
//                           + strerror(errno));
//     }
//      system((std::string{"systemctl "} + systemctlActions.at(action) + " " + processName + SERVICE_EXTENSION).c_str());
        execl("systemctl", "systemctl", systemctlActions.at(action).c_str(), (processName + SERVICE_EXTENSION).c_str(), nullptr);
  }

  return process;
}

std::vector<std::string> ApplicationHandler::getArgumentsList(const ProcessInfo& process) const
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
