#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <iostream>
#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>

namespace ExecutionManager
{

class ExecutionManager final: public ExecutionManagement::Server
{
public:
  int start();
private:
  using ApplicationState = ::ApplicationStateManagement::ApplicationState;

  ::kj::Promise<void> 
  reportApplicationState
    (ReportApplicationStateContext context) override;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP