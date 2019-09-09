#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

namespace MachineStateManager {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(std::atomic<bool> &terminate)
          : machineStateClient(std::make_unique<MachineStateClient>("unix:/tmp/execution_management")),
            m_currentState{std::make_unique<Init>(*this)},
            m_terminateApp{terminate}
{
  m_currentState->enter();
}

void MachineStateManager::transitToNextState()
{
  m_currentState->leave();
  m_currentState = m_currentState->handleTransition();
  m_currentState->enter();
}

bool MachineStateManager::isTerminating() const
{
    return m_terminateApp;
}

void MachineStateManager::setMachineState(std::string state, int timeout)
{
  machineStateClient->SetMachineState(state, timeout);
}

StateError MachineStateManager::registerMsm(const char* applicationName, int timeout)
{
  return machineStateClient->Register(applicationName, timeout);
}

void MachineStateManager::reportApplicationState(ApplicationState state)
{
  m_applState.ReportApplicationState(state);
}

}