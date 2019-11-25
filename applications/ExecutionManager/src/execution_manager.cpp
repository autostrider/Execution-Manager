#include "execution_manager.hpp"
#include <constants.hpp>
#include <logger.hpp>

#include <iostream>

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
    m_currentComponentState{},
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

bool ExecutionManager::isConfirmAvailable()
{
  return m_activeProcesses == m_allowedProcessesForState[m_pendingState];
}

void ExecutionManager::checkAndSendConfirm()
{
  if (isConfirmAvailable())
  {
    confirmState(StateError::K_SUCCESS);
  }
}

void ExecutionManager::changeComponentsState()
{
  if (m_pendingState == MACHINE_STATE_SUSPEND)
  {
    m_currentComponentState = COMPONENT_STATE_OFF;
  }
  else
  {
    m_currentComponentState = COMPONENT_STATE_ON;
  }

  for(auto& component : m_registeredComponents)
  {
    m_componentPendingConfirms.emplace(component);
  }
}

void
ExecutionManager::reportApplicationState(
    const std::string& appName, AppState state)
{
  LOG << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application "
      << appName
      << " received.";
  
  switch (state)
  {
  case AppState::kShuttingDown:
    m_activeProcesses.erase(appName);
    break;
  case AppState::kRunning:
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

  startApplicationsForState();
  
  if (m_pendingState == MACHINE_STATE_SUSPEND ||
      m_pendingState == MACHINE_STATE_RUNNING)
  {
    changeComponentsState();
  }
  
  if (!isConfirmAvailable() ||
      !m_componentPendingConfirms.empty())
  {
    LOG << "Machine state change to \""
        << m_pendingState
        << "\" requested.";
  }
  else
  {
    startApplicationsForState();
  }

  checkAndSendConfirm();

  return StateError::K_SUCCESS;
}

void ExecutionManager::registerComponent(std::string component)
{
  m_registeredComponents.emplace(component);
}

ComponentClientReturnType
ExecutionManager::getComponentState
(std::string component, ComponentState& state) const
{
  auto iter = m_registeredComponents.find(component);

  if(iter != m_registeredComponents.cend())
  {
    state = m_currentComponentState;
    return ComponentClientReturnType::K_SUCCESS;
  }
  else
  {
    return ComponentClientReturnType::K_GENERAL_ERROR;
  }
}

void ExecutionManager::confirmComponentState
(std::string component, ComponentState state, ComponentClientReturnType status)
{
  if (m_componentPendingConfirms.empty())
  {
    return;
  }

  if (status == ComponentClientReturnType::K_GENERAL_ERROR ||
      status == ComponentClientReturnType::K_INVALID)
  {
    LOG << "Confirm component state are faild with error: "
        << static_cast<int>(status) << ".";
  }

  m_componentPendingConfirms.erase(component);

  if (isConfirmAvailable() &&
      m_componentPendingConfirms.empty())
    {
     confirmState(StateError::K_SUCCESS);
    }
}

} // namespace ExecutionManager
