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

      m_registeredComponents.erase(app->first);
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

void ExecutionManager::changeComponentsState()
{
  ComponentState pendingComponentsState;

  if (m_pendingState == MACHINE_STATE_SUSPEND)
  {
    pendingComponentsState = COMPONENT_STATE_OFF;
  }
  else if (m_pendingState == MACHINE_STATE_RUNNING)
  {
    pendingComponentsState = COMPONENT_STATE_ON;
  }  

  for(auto& component : m_registeredComponents)
  {
    if(component.second != pendingComponentsState)
    {
      m_componentPendingConfirmsToBeReceived.emplace(component.first);
      component.second = pendingComponentsState;
    }
  }
}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{
  LOG << "State \"" << applicationStateNames[static_cast<uint16_t>(state)]
      << "\" for application with pid "
      << processId
      << " received.";

  if (m_stateConfirmToBeReceived.empty() &&
      m_componentPendingConfirmsToBeReceived.empty())
  {
    return;
  }

  if (state != AppState::kInitializing)
  {
    m_stateConfirmToBeReceived.erase(processId);

    if (m_stateConfirmToBeReceived.empty() &&
        m_componentPendingConfirmsToBeReceived.empty())
    {
     confirmState(StateError::kSuccess);
    }
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
    return StateError::kInvalidState;
  }
  // else if (m_stateConfirmToBeReceived.empty())
  // {
  //   return StateError::K_INVALID_REQUEST;
  // }
  else if (state == m_currentState)
  {
    return StateError::kInvalidState;
  }

  m_pendingState = state;

  killProcessesForState();

  startApplicationsForState();
  
  if (m_pendingState == MACHINE_STATE_SUSPEND ||
      m_pendingState == MACHINE_STATE_RUNNING)
  {
    changeComponentsState();
  }
  
  if (!m_stateConfirmToBeReceived.empty() ||
      !m_componentPendingConfirmsToBeReceived.empty())
  {
    LOG << "Machine state change to \""
        << m_pendingState
        << "\" requested.";
  }
  else
  {
    confirmState(StateError::kSuccess);
  }

  return StateError::kSuccess;
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
    return ComponentClientReturnType::kSuccess;
  }
  else
  {
    return ComponentClientReturnType::kInvalid;
  }
}

void ExecutionManager::confirmComponentState
(std::string component, ComponentState state, ComponentClientReturnType status)
{
  if (m_componentPendingConfirmsToBeReceived.empty())
  {
    return;
  }

  m_componentPendingConfirmsToBeReceived.erase(component);

  if (m_stateConfirmToBeReceived.empty() &&
      m_componentPendingConfirmsToBeReceived.empty())
    {
     confirmState(StateError::kSuccess);
    }
}

} // namespace ExecutionManager
