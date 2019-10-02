#ifndef EXECUTIONMANAGERCLIENTMOCK_HPP
#define EXECUTIONMANAGERCLIENTMOCK_HPP

#include "i_execution_manager_client.hpp"

#include "gmock/gmock.h"

using StateError = ::MachineStateManagement::StateError;

class ExecutionManagerClientMock :
  public ExecutionManagerClient::IExecutionManagerClient
{
public:
  MOCK_METHOD(void, confirm, (StateError status));
};

#endif // EXECUTIONMANAGERCLIENTMOCK_HPP