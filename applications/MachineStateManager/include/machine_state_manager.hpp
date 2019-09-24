#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <machine_state_client.h>
#include <i_adaptive_app.hpp>
#include <i_state_factory.hpp>
#include <i_application_state_client_wrapper.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <capnp/ez-rpc.h>

namespace MSM
{

class MachineStateManager : public api::IAdaptiveApp
{
public:
  MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                      std::unique_ptr<api::IApplicationStateClientWrapper> client);
  ~MachineStateManager() override = default;

  void init() override;
  void run() override;
  void terminate() override;

  api::MachineStateClient::StateError setMachineState(std::string);
  api::MachineStateClient::StateError registerMsm(const std::string&);
  void reportApplicationState(api::ApplicationStateClient::ApplicationState) override;

private:
  void transitToNextState(IAdaptiveApp::FactoryFunc nextState) override;

  std::unique_ptr<api::MachineStateClient> machineStateClient;
  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;
};

} // namespace MSM

#endif // MACHINE_STATE_MANAGER_HPP