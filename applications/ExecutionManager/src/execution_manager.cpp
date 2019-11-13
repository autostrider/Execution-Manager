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
  ComponentState pendingComponentsState =
    (m_pendingState == MACHINE_STATE_SUSPEND) ? COMPONENT_STATE_OFF :
                                                COMPONENT_STATE_ON;

  for(auto& component : m_registeredComponents)
  {
    if(component.second != pendingComponentsState)
    {
      m_componentConfirmToBeReceived.emplace(component.first);
      component.second = pendingComponentsState;
    }
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

  startApplicationsForState();

  changeComponentsState();

  if (!isConfirmAvailable() ||
      !m_componentConfirmToBeReceived.empty())
  {
    LOG << "Machine state change to \""
        << m_pendingState
        << "\" requested.";
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

void ExecutionManager::registerComponent(std::string component)
{
  m_registeredComponents.emplace(std::make_pair(component, COMPONENT_STATE_ON));
}

ComponentClientReturnType
ExecutionManager::getComponentState
(std::string component, ComponentState& state) const
{
  auto iter = m_registeredComponents.find(component);

  if(iter != m_registeredComponents.cend())
  {
    state = iter->second;
    return ComponentClientReturnType::K_SUCCESS;
  }
  else
  {
    return ComponentClientReturnType::K_INVALID;
  }
}

void ExecutionManager::confirmComponentState
(std::string component, ComponentState state, ComponentClientReturnType status)
{
  if (m_componentConfirmToBeReceived.empty())
  {
    return;
  }

  m_componentConfirmToBeReceived.erase(component);

  if (isConfirmAvailable() &&
      m_componentConfirmToBeReceived.empty())
    {
     confirmState(StateError::K_SUCCESS);
    }
}

} // namespace ExecutionManager
