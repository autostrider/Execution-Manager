#include "machine_state_manager.hpp"
#include "socket_server.hpp"
#include <i_state_factory.hpp>
#include <i_manifest_reader.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <constants.hpp>
#include <keyvaluestorage.h>
#include <logger.hpp>

namespace MSM {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(
        std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<api::IApplicationStateClientWrapper> appStateClient,
        std::unique_ptr<api::IMachineStateClientWrapper> machineClient,
        std::unique_ptr<ExecutionManager::IManifestReader> manifestReader,
        std::unique_ptr<ISocketServer> socketServer,
        std::unique_ptr<per::KeyValueStorageBase> persistentStorage) :
        m_machineStateClient(std::move(machineClient)),
        m_factory{std::move(factory)},
        m_currentState{nullptr},
        m_appStateClient{std::move(appStateClient)},
        m_newStatesProvider{std::move(socketServer)},
        m_availableStates{manifestReader->getMachineStates()}
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
                std::bind(&api::IStateFactory::createRun,
                          m_factory.get(),
                          std::placeholders::_1));
}

void MachineStateManager::terminate()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createShutDown,
                          m_factory.get(),
                          std::placeholders::_1));
}

void MachineStateManager::performAction()
{

}

void MachineStateManager::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}

void MachineStateManager::reportApplicationState(ApplicationState state)
{
    m_appStateClient->ReportApplicationState(state);
}

StateError MachineStateManager::setMachineState(const std::string& state)
{
  if (MACHINE_STATE_SHUTTINGDOWN == state)
  {
    return m_machineStateClient->SetMachineState(state, NO_TIMEOUT);
  }

  return m_machineStateClient->SetMachineState(state, DEFAULT_RESPONSE_TIMEOUT);
}

StateError MachineStateManager::registerMsm(const std::string& applicationName)
{
  return m_machineStateClient->Register(applicationName.c_str(),
                                        DEFAULT_RESPONSE_TIMEOUT);
}

std::string MachineStateManager::getNewState()
{
  std::string newState = m_newStatesProvider->getData();

  while(m_availableStates.cend() ==
          std::find(m_availableStates.cbegin(),
                  m_availableStates.cend(),
                   newState))
  {
    LOG << "Invalid state received: " << newState;
    newState = m_newStatesProvider->getData();
  }

  return newState;
}

void MachineStateManager::startServer()
{
  m_newStatesProvider->startServer();
}

void MachineStateManager::closeServer()
{
  m_newStatesProvider->closeServer();
}

} // namespace MSM
