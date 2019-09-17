#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <capnp/ez-rpc.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <machine_state_client.h>
#include <application_state_client.h>

namespace MachineStateManager
{

class MachineStateManager
{
public:
  MachineStateManager();
  ~MachineStateManager() = default;
  void start();

private:
  using StateError = api::MachineStateClient::StateError;

private:
  const char* applicationName = "Machine state manager";
  const int defaultTimeout = 300000;

  api::MachineStateClient m_machineStateClient;
  api::ApplicationStateClient m_appClient;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP