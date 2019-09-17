#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <capnp/ez-rpc.h>
#include <machine_state_client.h>

#include <i_adaptive_app.hpp>
#include <i_state_factory.hpp>
#include <i_application_state_client_wrapper.hpp>

namespace MachineStateManager
{

class MachineStateManager : public api::IAdaptiveApp
{
public:
  MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                      std::unique_ptr<api::IApplicationStateClientWrapper> client);
  ~MachineStateManager() = default;

  void init() override;
  void run() override;
  void terminate() override;

  void setMachineState(std::string, int);
  api::MachineStateClient::StateError registerMsm(const char*, int);
  void reportApplicationState(api::ApplicationStateClient::ApplicationState) override;

private:
  void transitToNextState(IAdaptiveApp::FactoryFunc nextState);

  using StateError = api::MachineStateClient::StateError;

  std::unique_ptr<api::MachineStateClient> machineStateClient;

  std::unique_ptr<api::IStateFactory> m_factory;
  std::unique_ptr<api::IState> m_currentState;
  std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP