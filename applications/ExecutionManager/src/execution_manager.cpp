#include "execution_manager.hpp"
#include <constants.hpp>
#include <common.hpp>
#include <logger.hpp>

#include <iostream>
#include <thread>
#include <future>

using namespace constants;

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
  std::unique_ptr<IApplicationHandler> applicationHandler)
  : m_appHandler{std::move(applicationHandler)},
    m_activeProcesses{},
    m_allowedProcessesForState{reader->getStatesSupportedByApplication()},
    m_currentState{},
    m_pendingState{},
    m_currentComponentState{},
    m_machineManifestStates{reader->getMachineStates()},
    m_componentStateUpdateSbscrs{}
{
  filterStates();
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

bool ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = m_allowedProcessesForState.find(m_pendingState);
  std::vector<ProcName> appsToBeStarted;

  if (allowedApps != m_allowedProcessesForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
    {
      if (m_activeProcesses.find(executableToStart) ==
          m_activeProcesses.cend())
      {
        appsToBeStarted.push_back(executableToStart);
      }
    }
  }
  
  if (m_pendingState == MACHINE_STATE_STARTUP)
  {
    std::future<bool> isMsmAlive = std::async(std::launch::async, [this, &appsToBeStarted]()
    {
      const int timesToPollApps = 6;
      const std::chrono::seconds oneSecond{1};
      for (int i = 0; i < timesToPollApps; ++i)
      {
        std::this_thread::sleep_for(oneSecond);
        for (const auto& executableToStart : appsToBeStarted)
        {
          if (m_appHandler->isActiveProcess(executableToStart) &&
              executableToStart.find("msm") != std::string::npos)
          {
            return true;
          }
        }
      }
      return false;
    });

    for (const auto& executableToStart : appsToBeStarted)
    {
      startApplication(executableToStart);
    }
    return isMsmAlive.get();
  }
  else
  {
    for (const auto& executableToStart : appsToBeStarted)
    {
      startApplication(executableToStart);
    }
  }
  return true;
}

void
ExecutionManager::confirmState(StateError status)
{
  m_rpcClient->confirm(status);
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

      m_componentStateUpdateSbscrs.erase(std::remove_if(m_componentStateUpdateSbscrs.begin(), 
                                                        m_componentStateUpdateSbscrs.end(),
                                                        [&](const std::string& currSubscr)
                                                        {return currSubscr == *app;}),
                                         m_componentStateUpdateSbscrs.end());
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
    LOG << "Trying to start: " << process;
    m_appHandler->startProcess(process);
}

bool ExecutionManager::isConfirmAvailable()
{
  bool result = false;
  {
    std::lock_guard<std::mutex> lk(m_activeProcessesMutex);
    result = (m_activeProcesses == m_allowedProcessesForState[m_pendingState]);
  }
  m_readyToTransitToNextState = result;

  return result;
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

  std::lock_guard<std::mutex> lk(m_componentPendingConfirmsMutex);

  for (auto subscriber : m_componentStateUpdateSbscrs)
  {
    m_rpcClient->SetComponentState(m_currentComponentState, subscriber);
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
  {
    std::lock_guard<std::mutex> lk(m_activeProcessesMutex);
    m_activeProcesses.erase(appName);
  }
    break;
  case AppState::kRunning:
  {
    std::lock_guard<std::mutex> lk(m_activeProcessesMutex);
    m_activeProcesses.insert(appName);
  }
    break;
  default:
    break;
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
  m_readyToTransitToNextState = false;
  m_componentConfirmsReceived = false;

  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                           m_machineManifestStates.cend(),
                           state);

  if (stateIt == m_machineManifestStates.end())
  {
    return StateError::kInvalidState;
  }
  else if (state == m_currentState)
  {
    return StateError::kInvalidState;
  }

  m_pendingState = state;

  killProcessesForState();

  if (!startApplicationsForState())
  {
    LOG << "Failed to start critical App!!!";
    return StateError::kInvalidRequest;
  }

  changeComponentsState();

  if (!isConfirmAvailable() ||
      !m_componentPendingConfirms.empty())
  {
    LOG << "Machine state change to \""
        << state
        << "\" requested.";
  }

  LOG << "-----Waiting for confirmation----";

  if (m_pendingState == MACHINE_STATE_SUSPEND ||
      m_pendingState == MACHINE_STATE_RUNNING)
  {
    while ((!m_readyToTransitToNextState) &&
            !m_componentConfirmsReceived)
    {
      m_readyToTransitToNextState = isConfirmAvailable();
      m_componentConfirmsReceived = m_componentPendingConfirms.empty();
    }
  }
  else
  {
    while (!m_readyToTransitToNextState)
    {
      m_readyToTransitToNextState = isConfirmAvailable();
    }
  }

  confirmState(StateError::kSuccess);
  m_currentState = m_pendingState;
  LOG  << "Machine state changed successfully to "
       << m_currentState << ".";

  return StateError::kSuccess;
}

} // namespace ExecutionManager
