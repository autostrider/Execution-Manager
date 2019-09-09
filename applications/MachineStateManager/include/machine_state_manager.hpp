#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <memory>
#include <capnp/ez-rpc.h>
#include <atomic>

#include "application_state_client.h"
#include "machine_state_client.h"

namespace MachineStateManager
{
class State;

class MachineStateManager
{
public:
  MachineStateManager(std::atomic<bool>&);
  ~MachineStateManager() = default;

  void transitToNextState();
  bool isTerminating() const;
  void setMachineState(std::string, int);
  api::MachineStateClient::StateError registerMsm(const char*, int);
  void reportApplicationState(api::ApplicationStateClient::ApplicationState);

private:
  std::unique_ptr<api::MachineStateClient> machineStateClient;
  std::unique_ptr<State> m_currentState;
  std::atomic<bool>& m_terminateApp;
  api::ApplicationStateClient m_applState;
};

}

#endif // MACHINE_STATE_MANAGER_HPP