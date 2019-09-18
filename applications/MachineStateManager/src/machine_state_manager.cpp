#include "machine_state_manager.hpp"
#include <constants.hpp>

namespace MachineStateManager {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
        machineStateClient(std::make_unique<MachineStateClient>(SOCKET_NAME)),
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

void MachineStateManager::setMachineState(std::string state, int timeout)
{
  machineStateClient->SetMachineState(state, timeout);
}

StateError MachineStateManager::registerMsm(const std::string& applicationName, uint timeout)
{
  return machineStateClient->Register(applicationName.c_str(), timeout);
}

} // namespace MachineStateManager