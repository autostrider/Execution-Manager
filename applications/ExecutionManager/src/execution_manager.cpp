#include "execution_manager.hpp"
#include <exception>
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

ExecutionManager::ExecutionManager(
  std::unique_ptr<IManifestReader> reader,
  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client)
  : m_activeApplications{}
  , m_allowedApplicationForState{reader->getStatesSupportedByApplication()}
  , m_currentState{}
  , m_machineManifestStates{reader->getMachineStates()}
  , m_machineStateClientAppName{}
  , m_rpcClient(std::move(client))
{
  filterStates();
}

void ExecutionManager::start()
{
  setMachineState(m_machineStateClientPid, defaultState);
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
  const auto& allowedApps = m_allowedApplicationForState.find(m_pendingState);

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

    m_stateConfirmToBeReceived.insert(processId);

    std::cout << "Application " << process.processName << "(" << processId << ") added" << std::endl;
  }
}

void
ExecutionManager::confirmState(StateError status)
{
  std::cout << "Machine state changed successfully to \""
            << m_pendingState
            << "\""
            << std::endl;

  m_currentState = m_pendingState;
  m_pendingState.clear();

  m_rpcClient->confirm(StateError::K_SUCCESS);
}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{
  std::cout << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
            << "\" for application with pid "
            << processId
            << " received"
            << std::endl;

  if(m_stateConfirmToBeReceived.empty())
  {
    return;
  }

  m_stateConfirmToBeReceived.erase(processId);

  if(m_stateConfirmToBeReceived.empty())
  {
   confirmState(StateError::K_SUCCESS);
  }
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

StateError
ExecutionManager::setMachineState(pid_t processId, string state)
{
  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                           m_machineManifestStates.cend(),
                           state);

  if(stateIt == m_machineManifestStates.end())
  {
    return StateError::K_INVALID_STATE;
  }

  if(processId != m_machineStateClientPid &&
     m_stateConfirmToBeReceived.empty())
  {
    return StateError::K_INVALID_REQUEST;
  }

  m_pendingState = state;

  killProcessesForState();

  startApplicationsForState();

  if(!m_stateConfirmToBeReceived.empty())
  {
    std::cout << "Machine state \""
              << m_pendingState
              << "\" requested"
              << std::endl;
  }
  else
  {
    std::cout << "GWEYGGGHHHHHH" << std::endl;
    confirmState(StateError::K_SUCCESS);
  }

  return StateError::K_SUCCESS;
}

} // namespace ExecutionManager
