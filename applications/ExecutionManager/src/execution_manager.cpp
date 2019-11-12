#include "execution_manager.hpp"
#include <constants.hpp>
#include <common.hpp>

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
  : m_appHandler{std::move(applicationHandler)},
    m_activeProcesses{},
    m_allowedProcessesForState{reader->getStatesSupportedByApplication()},
    m_currentState{},
    m_pendingState{},
    m_machineManifestStates{reader->getMachineStates()},
    m_rpcClient(std::move(client))
{
  filterStates();
}

void ExecutionManager::start()
{
  setMachineState(MACHINE_STATE_STARTUP);
}

void ExecutionManager::filterStates()
{
  for (auto app = m_allowedProcessesForState.begin();
       app != m_allowedProcessesForState.end();)
  {
    if (std::find(m_machineManifestStates.cbegin(),
                  m_machineManifestStates.cend(),
                  app->first) == m_machineManifestStates.cend())
    {
      app = m_allowedProcessesForState.erase(app);
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
          startApplication(executableToStart);
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
        processToBeKilled(*app, allowedApps->second))
    {
      m_appHandler->killProcess(*app);
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
  m_appHandler->startProcess(process);

  LOG << "Adaptive aplication \""
      << process
      << "\" was started by systemctl.";
}

void ExecutionManager::checkAndSendConfirm()
{
  if (m_activeProcesses == m_allowedProcessesForState[m_pendingState])
  {
    confirmState(StateError::K_SUCCESS);
  }
}

void
ExecutionManager::reportApplicationState(
    const std::string& appName, AppState state)
{
  LOG << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application " << appName
      << " received.";

  switch (state)
  {
  case AppState::SHUTTINGDOWN:
    m_activeProcesses.erase(appName);
    break;
  case AppState::RUNNING:
  case AppState::SUSPEND:
    m_activeProcesses.insert(appName);
    break;
  default:
    break;
  }

  checkAndSendConfirm();
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

  checkAndSendConfirm();

  return StateError::K_SUCCESS;
}

void 
ExecutionManager::suspend()
{
  LOG << "Suspend entered";
}

} // namespace ExecutionManager
