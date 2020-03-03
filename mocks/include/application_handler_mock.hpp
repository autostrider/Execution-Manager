#ifndef APPLICATION_HANDLER_MOCK_HPP
#define APPLICATION_HANDLER_MOCK_HPP

#include <i_application_handler.hpp>

#include "gmock/gmock.h"

namespace ExecutionManager
{
class ApplicationHandlerMock : public IApplicationHandler
{
public:
  MOCK_METHOD1(startProcess, bool(const std::string& application));
  MOCK_METHOD1(killProcess, bool(const std::string& processId));
  MOCK_METHOD1(isActiveProcess, bool(const std::string& serviceName));
};

} // namespace ExecutionManager

#endif // APPLICATION_HANDLER_MOCK_HPP
