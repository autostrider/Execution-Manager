#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <i_adaptive_app.hpp>
#include <i_machine_state_client_wrapper.hpp>
#include <client.hpp>


namespace per
{
  class KeyValueStorageBase;
}

namespace api
{
  class IManifestReader;
  class IStateFactory;
  class IServer;
}

namespace application_state
{
  class IApplicationStateClientWrapper;
}

namespace MSM
{

class MachineStateManager : public api::IAdaptiveApp
{
public:
  MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                      std::unique_ptr<application_state::IApplicationStateClientWrapper> appStateClient,
                      std::unique_ptr<machine_state_client::IMachineStateClientWrapper> machineClient,
                      std::unique_ptr<api::IManifestReader> manifestReader,
                      std::unique_ptr<base_client::Client> cilent,
                      std::unique_ptr<per::KeyValueStorageBase> persistentStorage);

  void init() override;
  void run() override;
  void terminate() override;
  void performAction() override;

  machine_state_client::StateError setMachineState(const std::string&);
  std::string getNewState();
  void saveReceivedState(const std::string& state);
  machine_state_client::StateError registerMsm(const std::string&);
  void
  reportApplicationState(application_state::ApplicationStateClient::ApplicationState) override;

private:
  void transitToNextState(api::IAdaptiveApp::FactoryFunc nextState) override;
  std::string getNewStateForStartRun();
  std::string recvState();

private:
  std::unique_ptr<machine_state_client::IMachineStateClientWrapper> m_machineStateClient;
  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<application_state::IApplicationStateClientWrapper> m_appStateClient;
  std::unique_ptr<base_client::Client> m_newStatesProvider;
  std::unique_ptr<per::KeyValueStorageBase> m_persistentStorage;
  std::vector<std::string> m_availableStates;
};

} // namespace MSM

#endif // MACHINE_STATE_MANAGER_HPP
