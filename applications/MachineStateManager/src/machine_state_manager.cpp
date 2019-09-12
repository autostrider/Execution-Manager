#include "machine_state_manager.hpp"

namespace MachineStateManager {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    api::IAdaptiveApp(std::move(factory), std::move(client)),
    machineStateClient(std::make_unique<MachineStateClient>
                        ("unix:/tmp/execution_management"))
{
    transitToNextState(
                std::bind(&api::IStateFactory::createInit, m_factory.get(), std::placeholders::_1)
                );
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
                std::bind(&api::IStateFactory::createTerminate, m_factory.get(), std::placeholders::_1)
                );
}

void MachineStateManager::setMachineState(std::string state, int timeout)
{
  machineStateClient->SetMachineState(state, timeout);
}

StateError MachineStateManager::registerMsm(const char* applicationName, int timeout)
{
  return machineStateClient->Register(applicationName, timeout);
}

} // namespace MachineStateManager