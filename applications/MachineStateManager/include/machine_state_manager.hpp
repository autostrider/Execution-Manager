#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <i_adaptive_app.hpp>
#include <i_state_factory.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <i_machine_state_client_wrapper.hpp>

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
                      std::unique_ptr<api::IApplicationStateClientWrapper> appStateClient,
                      std::unique_ptr<api::IMachineStateClientWrapper> machineClient);
  ~MachineStateManager() override = default;

  void init() override;
  void run() override;
  void terminate() override;
  void suspend() override;

  api::MachineStateClient::StateError setMachineState(const std::string&);
  api::MachineStateClient::StateError registerMsm(const std::string&);
  void reportApplicationState(api::ApplicationStateClient::ApplicationState) override;

private:
  void transitToNextState(api::IAdaptiveApp::FactoryFunc nextState) override;

private:
  std::unique_ptr<api::IMachineStateClientWrapper> m_machineStateClient;
  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<api::IApplicationStateClientWrapper> m_appStateClient;
};

} // namespace MSM

#endif // MACHINE_STATE_MANAGER_HPP
