#ifndef I_EXECUTION_MANAGER_CLIENT_HPP
#define I_EXECUTION_MANAGER_CLIENT_HPP

#include <execution_management.capnp.h>
#include <enums.pb.h>

#include <string>

using pComponentClientReturnType = enums::pComponentClientReturnType;
using StateError = ::MachineStateManagement::StateError;

namespace api
{

class IExecutionManagerClient
{
public:
  virtual void confirm(StateError status) = 0;
  virtual pComponentClientReturnType SetComponentState(std::string& state, 
                                                       std::string& componentName) = 0;
  virtual ~IExecutionManagerClient() noexcept(false) {}
};

}

#endif // I_EXECUTION_MANAGER_CLIENT_HPP
