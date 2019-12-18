#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <i_adaptive_app.hpp>

#include <i_machine_state_client_wrapper.hpp>

#include <capnp/ez-rpc.h>

class ISocketServer;

namespace ExecutionManager
{

class IManifestReader;

}

namespace per
{

class KeyValueStorageBase;

}

namespace api
{

class IStateFactory;
class IApplicationStateClientWrapper;

}

namespace MSM
{

class MachineStateManager : public api::IAdaptiveApp
{
public:
  MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                      std::unique_ptr<api::IApplicationStateClientWrapper> appStateClient,
                      std::unique_ptr<api::IMachineStateClientWrapper> machineClient,
                      std::unique_ptr<ExecutionManager::IManifestReader> manifestReader,
                      std::unique_ptr<ISocketServer> socketServer,
                      std::unique_ptr<per::KeyValueStorageBase> persistentStorage);

  void init() override;
  void run() override;
  void terminate() override;
  void performAction() override;

  api::MachineStateClient::StateError setMachineState(const std::string&);
  void startServer();
  void closeServer();
  std::string getNewState();
  void saveReceivedState(const std::string& state);
  api::MachineStateClient::StateError registerMsm(const std::string&);
  void
  reportApplicationState(api::ApplicationStateClient::ApplicationState) override;

private:
  void transitToNextState(api::IAdaptiveApp::FactoryFunc nextState) override;
  std::string getNewStateForStartRun();

private:
  std::unique_ptr<api::IMachineStateClientWrapper> m_machineStateClient;
  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<api::IApplicationStateClientWrapper> m_appStateClient;
  std::unique_ptr<ISocketServer> m_newStatesProvider;
  std::unique_ptr<per::KeyValueStorageBase> m_persistentStorage;
  std::vector<std::string> m_availableStates;
};

} // namespace MSM

#endif // MACHINE_STATE_MANAGER_HPP
