#ifndef APPLICATION_HANDLER_MOCK_HPP
#define APPLICATION_HANDLER_MOCK_HPP

#include "i_application_handler.hpp"

#include "gmock/gmock.h"

class ApplicationHandlerMock : public ExecutionManager::IApplicationHandler
{
public:
  MOCK_METHOD(pid_t, startProcess, (const ExecutionManager::ProcessInfo& application));
  MOCK_METHOD(void, killProcess, (pid_t processId));
};

#endif // APPLICATION_HANDLER_MOCK_HPP