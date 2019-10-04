#include "application_handler.hpp"

#include <csignal>
#include <exception>

namespace ExecutionManager
{

using std::runtime_error;

ApplicationHandler::ApplicationHandler(std::string path)
        : corePath{std::move(path)}
{ }

pid_t ApplicationHandler::startProcess(const ProcessInfo& process)
{
  pid_t processId = fork();

  if (!processId)
  {
    // child process
    const auto processPath = corePath
                           + process.createRelativePath();

    auto arguments = getArgumentsList(process);
    auto applicationArguments = convertToNullTerminatingArgv(arguments);
    int res = execv(processPath.c_str(), applicationArguments.data());

    if (res)
    {
      throw runtime_error(std::string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  }
  return processId;
}

void ApplicationHandler::killProcess(pid_t processId)
{
 kill(processId, SIGTERM);
}

void ApplicationHandler::suspend(pid_t processId)
{
 kill(processId, SIGINT);
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

std::vector<char*> ApplicationHandler::convertToNullTerminatingArgv(std::vector<std::string>& vectorToConvert) const
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