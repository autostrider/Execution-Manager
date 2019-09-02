#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <capnp/ez-rpc.h>
#include <machine_state_client.h>

namespace MachineStateManager
{

class MachineStateManager
{
  using StateError = api::MachineStateClient::StateError;
  void print(StateError);

public:
  MachineStateManager();
  ~MachineStateManager() = default;
  int32_t start();

private:
  std::unique_ptr<api::MachineStateClient> machineStateClient;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP