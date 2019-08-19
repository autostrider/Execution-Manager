#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <iostream>
#include <string>
#include <csignal>
#include <functional>
#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>
#include <chrono>
#include <thread>

namespace ExecutionManager
{

class ExecutionManager final: public ExecutionManagement::Server
{
public:
  int32_t start();

private:
  using ApplicationState = ::ApplicationStateManagement::ApplicationState;
  using StateError = ::MachineStateManagement::StateError;

  ::kj::Promise<void>
  reportApplicationState
    (ReportApplicationStateContext context) override;

  ::kj::Promise<void>
  register_(RegisterContext context) override;

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override;

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override;

private:
  std::string machineState{"kRunning"};

  std::string machineStateClientAppName;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP