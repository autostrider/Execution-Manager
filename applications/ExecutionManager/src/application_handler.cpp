#include "application_handler.hpp"

#include <csignal>
#include <exception>
#include <logger.hpp>

namespace ExecutionManager
{

using std::runtime_error;

ProcessHandler::ProcessHandler(std::unique_ptr<IOsInterface> syscalls,
                                       std::string path)
        : corePath{std::move(path)},
          m_syscalls{std::move(syscalls)}
{ }

pid_t ProcessHandler::startProcess(const ProcessInfo& process)
{
  pid_t processId = m_syscalls->fork();

  if (!processId)
  {
    // child process
    const auto processPath = corePath
                           + process.createRelativePath();

    auto arguments = getArgumentsList(process);
    auto applicationArguments = convertToNullTerminatingArgv(arguments);
    int res = m_syscalls->execv(processPath.c_str(),
                                applicationArguments.data());

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

void ProcessHandler::killProcess(pid_t processId)
{
  m_syscalls->kill(processId, SIGTERM);
}

std::vector<std::string> ProcessHandler::getArgumentsList(const ProcessInfo& process) const
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
ProcessHandler::convertToNullTerminatingArgv(
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




ApplicationHandler::ApplicationHandler(std::unique_ptr<IProcessHandler> processHandler,
                                       std::map<MachineState, std::vector<ProcessInfo> > allowedProcessesForState) :
    m_ProcessHandler{std::move(processHandler)},
    m_allowedProcessesForState{allowedProcessesForState},
    m_activeProcesses{}
{

}

void ApplicationHandler::filterStates(std::vector<MachineState>& machineManifestStates)
{
    for (auto app = m_activeProcesses.begin();
         app != m_activeProcesses.end();)
    {
      if (std::find(machineManifestStates.cbegin(),
                    machineManifestStates.cend(),
                    app->first) == machineManifestStates.cend())
      {
        app = m_activeProcesses.erase(app);
      }
      else
      {
        app++;
      }
    }
}

void ApplicationHandler::startApplication(const ProcessInfo& process,
                                          std::set<pid_t>& stateConfirmToBeReceived)
{
    pid_t processId = m_ProcessHandler->startProcess(process);
    m_activeProcesses.insert({process.processName, processId});

    stateConfirmToBeReceived.insert(processId);

    LOG << "Adaptive aplication \""
        << process.applicationName
        << "\" with pid "
        << processId
        << " started.";
}

void ApplicationHandler::startApplicationsForState(std::set<pid_t>& stateConfirmToBeReceived, MachineState& pendingState)
{
    const auto& allowedApps = m_allowedProcessesForState.find(pendingState);

    if (allowedApps != m_allowedProcessesForState.cend())
    {
      for (const auto& executableToStart: allowedApps->second)
      {
        if (m_activeProcesses.find(executableToStart.processName) ==
            m_activeProcesses.cend())
        {

          try
          {
            startApplication(executableToStart, stateConfirmToBeReceived);
          }
          catch (const runtime_error& err)
          {
            LOG << err.what() << ".";
          }
        }
      }
    }
}

void ApplicationHandler::killProcessesForState(std::set<pid_t>& stateConfirmToBeReceived, MachineState& pendingState)
{
    auto allowedApps = m_allowedProcessesForState.find(pendingState);

    for (auto app = m_activeProcesses.cbegin(); app != m_activeProcesses.cend();)
    {
      if (allowedApps == m_allowedProcessesForState.cend() ||
          processToBeKilled(app->first, allowedApps->second))
      {
        m_ProcessHandler->killProcess(app->second);
        stateConfirmToBeReceived.insert(app->second);

        app = m_activeProcesses.erase(app);
      }
      else
      {
        app++;
      }
    }
}

bool ApplicationHandler::processToBeKilled(const std::string& app, const std::vector<ProcessInfo>& allowedApps)
{
    auto it = std::find_if(allowedApps.cbegin(),
                       allowedApps.cend(),
                       [&app](auto& listItem)
      { return app == listItem.processName; });

    return (it  == allowedApps.cend());
}

} // namespace ExecutionManager
