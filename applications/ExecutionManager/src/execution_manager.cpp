#include "execution_manager.hpp"
#include <constants.hpp>

#include <iostream>
#include <logger.hpp>

namespace ExecutionManager
{

using std::runtime_error;

namespace {
  const std::vector<std::string> applicationStateNames{AA_STATE_INIT,
                                                       AA_STATE_RUNNING,
                                                       AA_STATE_SHUTDOWN,
                                                       AA_STATE_SUSPEND};
} // anonymous namespace

ExecutionManager::ExecutionManager(
  std::unique_ptr<IManifestReader> reader,
  std::unique_ptr<IApplicationHandler> applicationHandler,
  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client)
  : appHandler{std::move(applicationHandler)},
    m_activeProcesses{},
    m_allowedProcessesForState{reader->getStatesSupportedByApplication()},
    m_currentState{},
    m_pendingState{},
    m_machineManifestStates{reader->getMachineStates()},
    m_machineStateClientAppName{},
    m_rpcClient(std::move(client))
{
  filterStates();
}

void ExecutionManager::start()
{
  setMachineState(m_machineStateClientPid, MACHINE_STATE_STARTUP);
}

void ExecutionManager::filterStates()
{
  for (auto app = m_activeProcesses.begin();
       app != m_activeProcesses.end();)
  {
    if (std::find(m_machineManifestStates.cbegin(),
                  m_machineManifestStates.cend(),
                  app->first) == m_machineManifestStates.cend())
    {
      app = m_activeProcesses.erase(app);
    }
    else
    {
      app++;
    }
  }
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = m_allowedProcessesForState.find(m_pendingState);

  if (allowedApps != m_allowedProcessesForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
    {
      if (m_activeProcesses.find(executableToStart.processName) ==
          m_activeProcesses.cend())
      {

        try
        {
          startApplication(executableToStart);
        }
        catch (const runtime_error& err)
        {
          LOG << err.what() << ".";
        }
      }
    }
  }
}

void
ExecutionManager::confirmState(StateError status)
{
  m_currentState = m_pendingState;

  m_rpcClient->confirm(status);

  LOG  << "Machine state changed successfully to "
       << m_pendingState << ".";

  m_pendingState.clear();
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = m_allowedProcessesForState.find(m_pendingState);

  for (auto app = m_activeProcesses.cbegin(); app != m_activeProcesses.cend();)
  {
    if (allowedApps == m_allowedProcessesForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      appHandler->killProcess(app->second);
      m_stateConfirmToBeReceived.insert(app->second);

      app = m_activeProcesses.erase(app);
    }
    else
    {
      app++;
    }
  }
}

bool ExecutionManager::processToBeKilled(
  const std::string& app,
  const std::vector<ProcessInfo>& allowedApps)
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
  m_activeProcesses.insert({process.processName, processId});

  m_stateConfirmToBeReceived.insert(processId);

  LOG << "Adaptive aplication \""
      << process.applicationName
      << "\" with pid "
      << processId
      << " started.";
}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{
  LOG << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application with pid "
      << processId
      << " received.";

  if (m_stateConfirmToBeReceived.empty())
  {
    return;
  }

  if ((state == AppState::RUNNING) || (state == AppState::SHUTTINGDOWN))
  {
    m_stateConfirmToBeReceived.erase(processId);

    if (m_stateConfirmToBeReceived.empty())
    {
     confirmState(StateError::K_SUCCESS);
    }
  }
}

bool
ExecutionManager::registerMachineStateClient(pid_t processId, std::string appName)
{
  if (m_machineStateClientPid == -1 ||
      m_machineStateClientPid == processId)
  {
    m_machineStateClientPid = processId;
    m_machineStateClientAppName = appName;

    LOG << "State Machine Client \""
        << m_machineStateClientAppName
        << "\" with pid "
        << m_machineStateClientPid
        << " registered.";

    return true;
  }

  LOG << "State Machine Client \""
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
  LOG << "GetMachineState request received.";

  return m_currentState;
}

StateError
ExecutionManager::setMachineState(pid_t processId, std::string state)
{
  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                           m_machineManifestStates.cend(),
                           state);

  if (stateIt == m_machineManifestStates.end())
  {
    return StateError::K_INVALID_STATE;
  }

  if (processId != m_machineStateClientPid &&
      m_stateConfirmToBeReceived.empty())
  {
    return StateError::K_INVALID_REQUEST;
  }

  m_pendingState = state;

 if(m_pendingState == AA_STATE_SUSPEND)
  {
    suspend();
  }
  else
  {
    killProcessesForState();

    startApplicationsForState();
  }

  if (!m_stateConfirmToBeReceived.empty())
  {
    LOG << "Machine state change to  \""
        << m_pendingState
        << "\" requested.";
  }
  else
  {
    confirmState(StateError::K_SUCCESS);
  }

  return StateError::K_SUCCESS;
}

void 
ExecutionManager::suspend()
{
  for (auto app = m_activeProcesses.cbegin(); app != m_activeProcesses.cend(); app++)
  {
    appHandler->suspend(app->second);
    m_stateConfirmToBeReceived.insert(app->second);
  } 
}

} // namespace ExecutionManager
