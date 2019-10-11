#ifndef EXECUTION_MANAGER_SERVER_HPP
#define EXECUTION_MANAGER_SERVER_HPP

#include "execution_manager.hpp"
#include "msm_handler.hpp"

namespace ExecutionManagerServer
{

using ComponentState = ExecutionManager::ComponentState;
using ComponentClientReturnType = ExecutionManager::ComponentClientReturnType;

class ExecutionManagerServer : public ExecutionManagement::Server
{
public:
  explicit
  ExecutionManagerServer
    (ExecutionManager::ExecutionManager& application,
     ExecutionManager::MsmHandler msmHandler);

private:
  ::kj::Promise<void>
  reportApplicationState(ReportApplicationStateContext context) override;

  ::kj::Promise<void>
  register_(RegisterContext context) override;

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override;

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override;

  ::kj::Promise<void>
  registerComponent(RegisterComponentContext context) override;

  ::kj::Promise<void>
  getComponentState(GetComponentStateContext context) override;

  ::kj::Promise<void>
  confirmComponentState(ConfirmComponentStateContext context) override;

private:
  ExecutionManager::ExecutionManager& m_em;
  ExecutionManager::MsmHandler m_msmHandler;
};

} // namespace ExecutionManagerServer

#endif // EXECUTION_MANAGER_SERVER_HPP