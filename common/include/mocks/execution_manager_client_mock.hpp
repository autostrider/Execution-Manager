#ifndef EXECUTION_MANAGER_CLIENT_MOCK_HPP
#define EXECUTION_MANAGER_CLIENT_MOCK_HPP

#include "i_execution_manager_client.hpp"

#include "gmock/gmock.h"

using StateError = ::MachineStateManagement::StateError;

class ExecutionManagerClientMock :
  public ExecutionManagerClient::IExecutionManagerClient
{
public:
  MOCK_METHOD(void, confirm, (StateError status));
};

#endif // EXECUTION_MANAGER_CLIENT_MOCK_HPP