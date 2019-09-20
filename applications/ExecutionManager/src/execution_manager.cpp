#include "execution_manager.hpp"

#include <iostream>
#include <logger.hpp>

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

const MachineState ExecutionManager::defaultState {"Starting-up"};

ExecutionManager::ExecutionManager(
  std::unique_ptr<IManifestReader> reader,
  std::unique_ptr<IApplicationHandler> applicationHandler,
  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client)
  : appHandler{std::move(applicationHandler)},
    m_activeApplications{},
    m_allowedApplicationForState{reader->getStatesSupportedByApplication()},
    m_currentState{},
    m_machineManifestStates{reader->getMachineStates()},
    m_machineStateClientAppName{},
    m_rpcClient(std::move(client))
{
  filterStates();
}

void ExecutionManager::start()
{
  setMachineState(m_machineStateClientPid, defaultState);
  //startApplicationsForState();
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
          LOG << "\t" << err.what() << ".";
        }
      }
    }
  }
}

void
ExecutionManager::confirmState(StateError status)
{
  m_currentState = m_pendingState;
  m_pendingState.clear();

  m_rpcClient->confirm(StateError::K_SUCCESS);

  LOG  << "\t" << "Machine state changed successfully to "
       << m_pendingState << ".";
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = m_allowedApplicationForState.find(m_currentState);

  for (auto app = m_activeApplications.cbegin(); app != m_activeApplications.cend();)
  {
    if (allowedApps == m_allowedApplicationForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      appHandler->killProcess(app->second);
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

void ExecutionManager::startApplication(const ProcessInfo& process)
{
  pid_t processId = appHandler->startProcess(process);
  m_activeApplications.insert({process.processName, processId});

  m_stateConfirmToBeReceived.insert(processId);
  
  LOG << "\t" << "Adaptive aplication \""
              << process.applicationName
              << "\" with pid "
              << processId
              << " started.";
}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{
  LOG << "\t" << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application with pid "
      << processId
      << " received.";

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

    LOG << "\t" << "State Machine Client \""
              << m_machineStateClientAppName
              << "\" with pid "
              << m_machineStateClientPid
              << " registered.";

    return true;
  }

  LOG << "\t" << "State Machine Client \""
      << appName
      << "\" registration failed"
      << "\" with pid "
      << processId
      << " registration failed.";

  return false;
}

MachineState
ExecutionManager::getMachineState(pid_t processId) const
{
  LOG << "\t" << "GetMachineState request received.";

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
    LOG << "\t" << "Machine state change to  \""
        << m_pendingState
        << "\" requested.";
  }
  else
  {
    confirmState(StateError::K_SUCCESS);

    LOG << "\t" << "HEREEEE ?????";

  }

  return StateError::K_SUCCESS;
/*
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
*/
}

} // namespace ExecutionManager
