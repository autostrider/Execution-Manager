#include "application_handler.hpp"

#include <csignal>
#include <exception>
#include <logger.hpp>
#include <fstream>
#include <thread>

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
     auto applicationArgs = convertToNullTerminatingArgv(arguments);
     std::string cmd;
     
     for (const auto& item: arguments)
     {
         cmd.append(item);
         cmd.append(" ");
     }
     
     int res = m_syscalls->execvp(systemCtl.c_str(),
                      applicationArgs.data());

     if (res)
     {
       throw runtime_error(std::string{"Error occured creating process: "}
                           + processName
                           + " "
                           + strerror(errno));
     }
    
  }

  return process;
}

//std::vector<std::string> ApplicationHandler::getArgumentsList(const ProcessInfo& process) const
//{
//  std::vector<std::string> arguments;
//  arguments.reserve(process.startOptions.size() + 1);

//  // insert app name
//  arguments.push_back(process.processName);

//  std::transform(process.startOptions.cbegin(),
//                 process.startOptions.cend(),
//                 std::back_inserter(arguments),
//                 [](const StartupOption& option)
//                 { return option.makeCommandLineOption(); });

//  return arguments;
//}

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
