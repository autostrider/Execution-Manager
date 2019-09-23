#ifndef MACHINE_STATE_CLIENT_H
#define MACHINE_STATE_CLIENT_H

#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <machine_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <kj/async-io.h>

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

  StateError Register(std::string appName, std::uint32_t timeout);
  StateError GetMachineState(std::uint32_t timeout, std::string& state);
  StateError SetMachineState(std::string state, std::uint32_t timeout);
private:
  capnp::EzRpcClient m_client;
  MachineStateManagement::Client m_clientApplication;
  kj::Timer& m_timer;

  pid_t m_pid;
};

}
#endif // MACHINE_STATE_CLIENT_H
