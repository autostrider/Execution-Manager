#ifndef EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP

#include <execution_management.capnp.h>
#include <string>

namespace ExecutionManagerClient
{

using StateError = ::MachineStateManagement::StateError;

class IExecutionManagerClient
{
public:
  virtual void confirm(StateError status) = 0;
  virtual StateManagement::ComponentClientReturnType SetComponentState(std::string& state, 
                                                                       std::string& componentName) = 0;
  virtual ~IExecutionManagerClient() noexcept(false) {}
};

}

#endif //EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP
