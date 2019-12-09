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
                      std::unique_ptr<ISocketServer> socketServer);

  void init() override;
  void run() override;
  void terminate() override;
  void performAction() override;

  api::MachineStateClient::StateError setMachineState(const std::string&);
  void startServer();
  void closeServer();
  std::string getNewState();
  api::MachineStateClient::StateError registerMsm(const std::string&);
  void
  reportApplicationState(api::ApplicationStateClient::ApplicationState) override;

  std::vector<std::string> states() const;

private:
  void transitToNextState(api::IAdaptiveApp::FactoryFunc nextState) override;

private:
  std::unique_ptr<api::IMachineStateClientWrapper> m_machineStateClient;
  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<api::IApplicationStateClientWrapper> m_appStateClient;
  std::unique_ptr<ISocketServer> m_newStatesProvider;
  std::vector<std::string> m_availableStates;
};

} // namespace MSM

#endif // MACHINE_STATE_MANAGER_HPP
