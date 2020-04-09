#include "machine_state_manager.hpp"
#include <i_state_factory.hpp>
#include <i_manifest_reader.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <constants.hpp>
#include <keyvaluestorage.h>
#include <kvstype.h>
#include <logger.hpp>
#include <machine_state_management.pb.h>
#include <any.pb.h>
#include <future>
#include <client_socket.hpp>

using namespace constants;

namespace
{
  const std::string STATE_KEY = "state";
}

namespace MSM
{

using machine_state_client::MachineStateClient;
using ApplicationState = application_state::ApplicationStateClient::ApplicationState;
using StateError = machine_state_client::StateError;

MachineStateManager::MachineStateManager(
        std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<application_state::IApplicationStateClientWrapper> appStateClient,
        std::unique_ptr<machine_state_client::IMachineStateClientWrapper> machineClient,
        std::unique_ptr<api::IManifestReader> manifestReader,
        std::unique_ptr<base_client::Client> client,
        std::unique_ptr<per::KeyValueStorageBase> persistentStorage) :
        m_machineStateClient(std::move(machineClient)),
        m_factory{std::move(factory)},
        m_currentState{nullptr},
        m_appStateClient{std::move(appStateClient)},
        m_newStatesProvider{std::move(client)},
        m_persistentStorage{std::move(persistentStorage)},
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
  return m_machineStateClient->SetMachineState(state, DEFAULT_RESPONSE_TIMEOUT);
}

StateError MachineStateManager::registerMsm(const std::string& applicationName)
{
  auto c = std::make_unique<base_client::Client>(MSM_STATES_SERVER,
                                                 std::make_unique<socket_handler::ClientSocket>());
  m_machineStateClient->setClient(std::move(c));
  return m_machineStateClient->Register(applicationName.c_str(), DEFAULT_RESPONSE_TIMEOUT);
}

std::string MachineStateManager::getNewState()
{
  std::string newState = getNewStateForStartRun();

  while(m_availableStates.cend() ==
          std::find(m_availableStates.cbegin(),
                  m_availableStates.cend(),
                   newState))
  {
    LOG << "Invalid state received: " << newState;
    newState = recvState();
  }

  return newState;
}

std::string MachineStateManager::getNewStateForStartRun()
{
  static bool firstRun = true;

  if (firstRun)
  {
    firstRun = false;
    auto lastState = m_persistentStorage->GetValue(STATE_KEY);

    if (per::KvsType::Type::kString == lastState.GetType()
          &&
        MACHINE_STATE_SHUTTINGDOWN != lastState.GetString())
    {
      return lastState.GetString();
    }
  }

  return recvState();
}

std::string MachineStateManager::recvState()
{
  google::protobuf::Any recvData;
  std::string data;

  do
  {
    if (m_newStatesProvider->receive(data) > 0)
    {
      recvData.ParseFromString(data);
        
      if(recvData.Is<MachineStateManagement::nextState>())
      {
        MachineStateManagement::nextState context;
        recvData.UnpackTo(&context);
        data = context.state();
        context = {};

        return data;
      }

      recvData = {};
      data = {};
    }
  } while (true);
}

void MachineStateManager::saveReceivedState(const std::string &state)
{
  std::async(std::launch::async, [&] {
      m_persistentStorage->SetValue(STATE_KEY, per::KvsType(state));
      m_persistentStorage->SyncToStorage();
  });
}

} // namespace MSM
