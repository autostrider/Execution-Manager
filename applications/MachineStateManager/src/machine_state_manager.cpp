#include <chrono>
#include <thread>

#include "machine_state_manager.hpp"
#include "application_state_client.h"


namespace MachineStateManager {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;

MachineStateManager::MachineStateManager(std::atomic<bool> &terminate)
          : machineStateClient(std::make_unique<MachineStateClient>("unix:/tmp/execution_management")),
            m_curentState{std::make_unique<Init>()},
            m_terminateApp{terminate},
            m_applState(std::make_unique<api::ApplicationStateClient>())
{
  m_curentState->enter(*this);
}

void MachineStateManager::transitToNextState()
{
    m_curentState = m_curentState->handleTransition(*this);
    m_curentState->enter(*this);
}

bool MachineStateManager::isTerminating() const
{
    return m_terminateApp;
}

void MachineStateManager::setMachineState(std::string state, int timeout)
{
  machineStateClient->SetMachineState(state, timeout);
}

MachineStateManager::StateError MachineStateManager::registerMsm(const char* applicationName, int timeout)
{
  return machineStateClient->Register(applicationName, timeout);
}

void MachineStateManager::reportStateToEm(ApplicationState state)
{
  m_applState->ReportApplicationState(state);
}

} // namespace MachineStateManager