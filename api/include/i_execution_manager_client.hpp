#ifndef I_EXECUTION_MANAGER_CLIENT_HPP
#define I_EXECUTION_MANAGER_CLIENT_HPP

#include <machine_state_management.pb.h>
#include <enums.pb.h>

#include <string>

namespace api
{

using StateError = MachineStateManagement::StateError;
using ComponentClientReturnType = enums::ComponentClientReturnType;

class IExecutionManagerClient
{
public:
  virtual void confirm(StateError status) = 0;
  virtual ComponentClientReturnType SetComponentState(std::string& state, 
                                                      std::string& componentName) = 0;
  virtual ~IExecutionManagerClient() noexcept(false) {}
};

}

#endif // I_EXECUTION_MANAGER_CLIENT_HPP
