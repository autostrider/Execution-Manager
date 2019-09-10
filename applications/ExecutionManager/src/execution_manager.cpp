#include "execution_manager.hpp"

#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <thread>
#include <signal.h>
#include <iostream>
#include <unistd.h>

namespace ExecutionManager
{

using std::runtime_error;
using std::string;

namespace {
  const char * applicationStateNames[] =
  {
    "Initializing",
    "Running",
    "Shuttingdown"
  };
} // anonymous namespace

const string ExecutionManager::corePath =
  string{"./bin/applications/"};

const MachineState ExecutionManager::defaultState {"Startup"};

ExecutionManager::ExecutionManager(std::unique_ptr<IManifestReader> reader)
  : m_activeApplications{}
  , m_allowedApplicationForState{reader->getStatesSupportedByApplication()}
  , m_currentState{}
  , m_machineManifestStates{reader->getMachineStates()}
  , m_machineStateClientAppName{}
{
  filterStates();
}

void ExecutionManager::start()
{
  startApplicationsForState();
}

void ExecutionManager::filterStates()
{
  for (auto app = m_activeApplications.begin();
       app != m_activeApplications.end();)
  {
    if (std::find(m_machineManifestStates.cbegin(),
                  m_machineManifestStates.cend(),
                  app->first) == m_machineManifestStates.cend())
    {
      app = m_activeApplications.erase(app);
    }
    else
    {
      app++;
    }
  }
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = m_allowedApplicationForState.find(m_currentState);

  if (allowedApps != m_allowedApplicationForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
    {
      if (m_activeApplications.find(executableToStart.processName) ==
          m_activeApplications.cend())
      {

        try
        {
          startApplication(executableToStart);
        }
        catch (const runtime_error& err)
        {
          std::cout << err.what() << std::endl;
        }
      }
    }
  }
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = m_allowedApplicationForState.find(m_currentState);

  for (auto app = m_activeApplications.cbegin(); app != m_activeApplications.cend();)
  {
    if (allowedApps == m_allowedApplicationForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      kill(app->second, SIGTERM);
      app = m_activeApplications.erase(app);
    }
    else
    {
      app++;
    }
  }
}

bool ExecutionManager::processToBeKilled(const string& app, const std::vector<ProcessInfo>& allowedApps)
{
  auto it = std::find_if(allowedApps.cbegin(),
                     allowedApps.cend(),
                     [&app](auto& listItem)
    { return app == listItem.processName; });

  return (it  == allowedApps.cend());
};

std::vector<std::string>
ExecutionManager::getArgumentsList(const ProcessInfo& process) const
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

std::vector<char *>
ExecutionManager::convertToNullTerminatingArgv(
    std::vector<std::string> &vectorToConvert)
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

void ExecutionManager::startApplication(const ProcessInfo& process)
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
      throw runtime_error(string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else {
    // parent process
    m_activeApplications.insert({process.processName, processId});
  }

}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{

  std::cout << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
            << "\" for application with pid "
            << processId
            << " received"
            << " received"
            << std::endl;
}

bool
ExecutionManager::registerMachineStateClient(pid_t processId, string appName)
{

  if (m_machineStateClientPid == -1 ||
      m_machineStateClientPid == processId)
  {
    m_machineStateClientPid = processId;
    m_machineStateClientAppName = appName;

    std::cout << "State Machine Client \""
              << m_machineStateClientAppName
              << "\" with pid "
              << m_machineStateClientPid
              << " registered"
              << std::endl;

    return true;
  }

  std::cout << "State Machine Client \""
            << appName
            << "\" registration failed"
            << "\" with pid "
            << processId
            << " registration failed"
            << std::endl;

  return false;
}

MachineState
ExecutionManager::getMachineState(pid_t processId) const
{
  std::cout << "getMachineState request received" << std::endl;

  return m_currentState;
}

bool
ExecutionManager::setMachineState(pid_t processId, string state)
{
  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                                 m_machineManifestStates.cend(),
                                 state);

  if (stateIt != m_machineManifestStates.cend() &&
      processId == m_machineStateClientPid)
  {
    m_currentState = state;

    killProcessesForState();

    startApplicationsForState();

    std::cout << "Machine state changed successfully to "
              << "\""
              << m_currentState
              << "\""
              << std::endl;

    return true;
  }

  std::cout << "Invalid machine state received - "
            << "\"" << state << "\""
            << std::endl;

  return false;
}

} // namespace ExecutionManager
