#ifndef MACHINE_STATE_CLIENT_H
#define MACHINE_STATE_CLIENT_H
#include <string>
#include <machine_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <kj/async-io.h>
#include <iostream>

namespace api {

class MachineStateClient
{
public:
  MachineStateClient(std::string path);
  ~MachineStateClient() = default;

  // K_SUCCESS
  // K_INVALID_STATE
  // K_INVALID_REQUEST
  // K_TIMEOUT
  using StateError = MachineStateManagement::StateError;

  StateError registerClient(std::string appName, std::uint32_t timeout);
  StateError getMachineState(std::uint32_t timeout, std::string& state);
  StateError setMachineState(std::string state, std::uint32_t timeout);
private:
  capnp::EzRpcClient client;
  MachineStateManagement::Client clientApplication;
  kj::Timer& timer;
};

}
#endif // MACHINE_STATE_CLIENT_H
