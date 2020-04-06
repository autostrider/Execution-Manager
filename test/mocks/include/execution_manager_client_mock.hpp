#ifndef EXECUTION_MANAGER_CLIENT_MOCK_HPP
#define EXECUTION_MANAGER_CLIENT_MOCK_HPP

#include <i_execution_manager_client.hpp>

#include "gmock/gmock.h"

namespace api
{

class ExecutionManagerClientMock :
  public IExecutionManagerClient
{
public:
  MOCK_METHOD(void, confirm, (StateError status));
  MOCK_METHOD(ComponentClientReturnType, SetComponentState, (std::string& state, std::string& componentName));
};

} // namespace api

#endif // EXECUTION_MANAGER_CLIENT_MOCK_HPP
