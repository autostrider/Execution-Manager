#ifndef EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP

#include <execution_management.capnp.h>


namespace ExecutionManagerClient
{

using StateError = ::MachineStateManagement::StateError;

class IExecutionManagerClient
{
public:
  virtual void confirm(StateError status) = 0;
};

}

#endif //EXECUTION_MANAGER_I_EXECUTION_MANAGER_CLIENT_HPP