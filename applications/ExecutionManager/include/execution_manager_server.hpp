#ifndef EXECUTION_MANAGER_SERVER_HPP
#define EXECUTION_MANAGER_SERVER_HPP

#include "execution_manager.hpp"

#include <kj/async-io.h>
#include <string>

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

  ExecutionManager::ExecutionManager& m_em;
};

} // namespace ExecutionManagerServer

#endif // EXECUTION_MANAGER_SERVER_HPP