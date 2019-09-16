#ifndef EXECUTION_MANAGER_SERVER_HPP
#define EXECUTION_MANAGER_SERVER_HPP

#include <string>
#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>
#include "execution_manager.hpp"

namespace ExecutionManagerServer
{

class ExecutionManagerServer : public ExecutionManagement::Server
{
public:

  explicit
  ExecutionManagerServer
    (ExecutionManager::ExecutionManager& application);
private:

  ::kj::Promise<void>
  reportApplicationState(ReportApplicationStateContext context) override;

  ::kj::Promise<void>
  register_(RegisterContext context) override;

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override;

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override;
private:

  using ApplicationState = ::ApplicationStateManagement::ApplicationState;
  using StateError = ::MachineStateManagement::StateError;

  ExecutionManager::ExecutionManager& m_em;
};

} // namespace ExecutionManagerServer

#endif // EXECUTION_MANAGER_SERVER_HPP