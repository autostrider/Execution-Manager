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
  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client)
  : appHandler{std::move(applicationHandler)},
    m_activeProcesses{},
    m_allowedProcessesForState{},
    m_currentState{},
    m_pendingState{},
    m_machineManifestStates{reader->getMachineStates()},
    m_rpcClient(std::move(client))
{
  const auto fullProcessesInfoInStates =
      reader->getStatesSupportedByApplication();
  for (const auto& processesInfoForState: fullProcessesInfoInStates)
  {
    std::set<ProcName> availableProcesses;
    std::transform(
        processesInfoForState.second.begin(),
          processesInfoForState.second.end(),
          std::inserter(availableProcesses, availableProcesses.begin()),
          [](auto item) { return item.processName; }
          );

    m_allowedProcessesForState.insert(
      {processesInfoForState.first, availableProcesses});
  }
  filterStates();
}

void ExecutionManager::start()
{
  setMachineState(MACHINE_STATE_STARTUP);
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
      if (m_activeProcesses.find(executableToStart) ==
          m_activeProcesses.cend())
      {

        try
        {
          startApplication(executableToStart);
        }
        catch (const runtime_error& err)
        {
          LOG << err.what();
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

  for (auto app = m_activeProcesses.cbegin();
       app != m_activeProcesses.cend();
       app++)
  {
    if (allowedApps == m_allowedProcessesForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      appHandler->killProcess(app->first);
      m_stateConfirmToBeReceived.insert(app->first);
    }
  }
}

bool ExecutionManager::processToBeKilled(const std::string& app,
  const std::set<ProcName> &allowedApps)
{
  auto it = std::find_if(allowedApps.cbegin(),
                     allowedApps.cend(),
                     [&app](auto& listItem)
    { return app == listItem; });

  return (it  == allowedApps.cend());
};

void ExecutionManager::startApplication(const ProcName& process)
{
  appHandler->startProcess(process);


  LOG << "Adaptive aplication \""
      << process
      << "\" was started by systemctl.";
}

void
ExecutionManager::reportApplicationState(
    pid_t processId, const std::string& appName, AppState state)
{
  LOG << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application " << appName << " with pid "
      << processId
      << " received.";

  if (AppState::SHUTTINGDOWN == state)
  {
    m_activeProcesses.erase(appName);
    m_stateConfirmToBeReceived.erase(appName);

  }

  if ((AppState::RUNNING == state) || (AppState::SUSPEND == state))
  {
    m_stateConfirmToBeReceived.insert(appName);
    m_activeProcesses.insert({appName, processId});
  }

  if (m_stateConfirmToBeReceived == m_allowedProcessesForState[m_pendingState])
  {
    confirmState(StateError::K_SUCCESS);
  }
}

MachineState
ExecutionManager::getMachineState() const
{
  LOG << "GetMachineState request received.";

  return m_currentState;
}

StateError
ExecutionManager::setMachineState(std::string state)
{
  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                           m_machineManifestStates.cend(),
                           state);

  if (stateIt == m_machineManifestStates.end())
  {
    return StateError::K_INVALID_STATE;
  }
  else if (state == m_currentState)
  {
    return StateError::K_INVALID_STATE;
  }

  m_pendingState = state;

  killProcessesForState();

  if(m_pendingState == AA_STATE_SUSPEND)
  {
    suspend();
  }
  else
  {
    startApplicationsForState();
  }

    LOG << "Machine state change to  \""
        << m_pendingState
        << "\" requested.";

    if (m_stateConfirmToBeReceived == m_allowedProcessesForState[m_pendingState])
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
    m_stateConfirmToBeReceived.insert(app->first);
  } 
}

} // namespace ExecutionManager
