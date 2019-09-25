#include "machine_state_manager.hpp"
#include <constants.hpp>

namespace MSM {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
        machineStateClient(std::make_unique<MachineStateClient>(IPC_PROTOCOL + EM_SOCKET_NAME)),
        m_factory{std::move(factory)},
        m_currentState{nullptr},
        m_appClient{std::move(client)}
{
}

void MachineStateManager::init()
{
    m_currentState = m_factory->createInit(*this);
    m_currentState->enter();
}

void MachineStateManager::run()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createRun, m_factory.get(), std::placeholders::_1)
                );
}

void MachineStateManager::terminate()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createShutDown, m_factory.get(), std::placeholders::_1)
                );
}

void MachineStateManager::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}

void MachineStateManager::reportApplicationState(ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}

StateError MachineStateManager::setMachineState(std::string state)
{
  if (MACHINE_STATE_SHUTTINGDOWN == state)
  {
    return machineStateClient->SetMachineState(state, NO_TIMEOUT);
  }

  return machineStateClient->SetMachineState(state, DEFAULT_RESPONSE_TIMEOUT);
}

StateError MachineStateManager::registerMsm(const std::string& applicationName)
{
  return machineStateClient->Register(applicationName.c_str(), DEFAULT_RESPONSE_TIMEOUT);
}

} // namespace MSM
