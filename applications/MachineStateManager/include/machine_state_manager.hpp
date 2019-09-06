#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <capnp/ez-rpc.h>
#include <atomic>

#include "machine_state_client.h"
#include "application_state_client.h"
#include "msm_state_machine.hpp"

namespace MachineStateManager
{
class State;

class MachineStateManager
{

public:
  using StateError = api::MachineStateClient::StateError;
  using ApplicationState = api::ApplicationStateClient::ApplicationState;

  MachineStateManager(std::atomic<bool>&);
  ~MachineStateManager() = default;
  void transitToNextState();
  bool isTerminating() const;
  void setMachineState(std::string, int);
  StateError registerMsm(const char*, int);
  void reportStateToEm(ApplicationState);

private:
  std::unique_ptr<api::MachineStateClient> machineStateClient;
  std::unique_ptr<State> m_curentState;
  std::atomic<bool>& m_terminateApp;
  std::unique_ptr<api::ApplicationStateClient> m_applState;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP