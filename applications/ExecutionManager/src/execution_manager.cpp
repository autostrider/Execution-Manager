#include "execution_manager.hpp"
#include <constants.hpp>
#include <common.hpp>
#include <i_os_interface.hpp>

#include <iostream>
#include <logger.hpp>

#include <thread>
#include <future>

namespace ExecutionManager
{

using std::runtime_error;

namespace {
const std::vector<std::string> applicationStateNames{AA_STATE_INIT,
            AA_STATE_RUNNING,
            AA_STATE_SHUTDOWN,
            AA_STATE_SUSPEND};
} // anonymous namespace

ExecutionManager::ExecutionManager(std::unique_ptr<IManifestReader> reader,
        std::unique_ptr<IApplicationHandler> applicationHandler,
        std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client,
        std::unique_ptr<IOsInterface> newAppsSycalls,
        std::unique_ptr<IOsInterface> aliveAppsSyscalls)
    : m_appHandler{std::move(applicationHandler)},
      m_activeProcesses{},
      m_failedApps{},
      m_allowedProcessesForState{reader->getStatesSupportedByApplication()},
      m_currentState{},
      m_pendingState{},
      m_currentComponentState{},
      m_machineManifestStates{reader->getMachineStates()},
      m_rpcClient(std::move(client)),
      m_aliveAppsObserver{std::move(aliveAppsSyscalls)},
      m_newAppObserver{std::move(newAppsSycalls)}
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
		std::set<ProcName> sumProcesses;
    {
        std::lock_guard<std::mutex> lk(m_activeProcessesMutex);
				std::set_union(m_activeProcesses.begin(), m_activeProcesses.end(),
						m_failedApps.begin(), m_failedApps.end(),
						std::inserter(sumProcesses, sumProcesses.begin()));
		}
		result = (sumProcesses == m_allowedProcessesForState[m_pendingState]);
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
        return StateError::K_INVALID_STATE;
    }
    else if (state == m_currentState)
    {
        return StateError::K_INVALID_STATE;
    }

    m_pendingState = state;

    killProcessesForState();

    if (!startApplicationsForState())
    {
        LOG << "Failed to start critical App!!!";
        return StateError::K_INVALID_REQUEST;
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
        while ((!m_readyToTransitToNextState)
               && ! m_componentConfirmsReceived)
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

    confirmState(StateError::K_SUCCESS);
    LOG << "-----Confirmation received------";
    m_currentState = m_pendingState;
    LOG  << "Machine state changed successfully to "
         << m_currentState << ".";

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
        m_componentConfirmsReceived = true;
        return;
    }
    if (status == ComponentClientReturnType::K_GENERAL_ERROR ||
            status == ComponentClientReturnType::K_INVALID)
    {
        LOG << "Confirm component state are failed with error: "
            << static_cast<int>(status) << ".";
    }
    else {
        std::lock_guard<std::mutex> lk(m_componentPendingConfirmsMutex);
        m_componentPendingConfirms.erase(component);
        m_componentConfirmsReceived = m_componentPendingConfirms.empty();
    }
}

void ExecutionManager::removeFailedToStartApp(const ProcName& app)
{
	std::lock_guard<std::mutex> lk{m_failedAppsMutex};
	m_failedApps.insert(app);
    m_newAppObserver.detach(app);
}

void ExecutionManager::removeFailedApp(const ProcName& app)
{
	std::lock_guard<std::mutex> ls{m_activeProcessesMutex};
	m_activeProcesses.erase(app);
    m_aliveAppsObserver.detach(app);
}

std::set<ProcName> ExecutionManager::getActiveApps() const
{
  return m_activeProcesses;
}

} // namespace ExecutionManager
