#ifndef EXECUTION_MANAGER_CLIENT_MOCK_HPP
#define EXECUTION_MANAGER_CLIENT_MOCK_HPP

#include <i_execution_manager_client.hpp>

#include "gmock/gmock.h"

namespace api
{

using StateError = ::MachineStateManagement::StateError;

class ExecutionManagerClientMock :
  public IExecutionManagerClient
{
public:
  MOCK_METHOD1(confirm, void(StateError status));
  MOCK_METHOD2(SetComponentState, StateManagement::ComponentClientReturnType(std::string& state, std::string& componentName));
};

} // namespace api

#endif // EXECUTION_MANAGER_CLIENT_MOCK_HPP