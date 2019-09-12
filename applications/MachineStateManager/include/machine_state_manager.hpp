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

namespace MachineStateManager
{

class MachineStateManager : public api::IAdaptiveApp
{
public:
  MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                      std::unique_ptr<api::IApplicationStateClientWrapper> client);
  ~MachineStateManager() = default;
  //int32_t start();

  void run() override;
  void terminate() override;

  void setMachineState(std::string, int);
  api::MachineStateClient::StateError registerMsm(const char*, int);
  //void reportApplicationState(api::ApplicationStateClient::ApplicationState);

private:
  using StateError = api::MachineStateClient::StateError;

  std::unique_ptr<api::MachineStateClient> machineStateClient;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP