#ifndef APPLICATION_HANDLER_MOCK_HPP
#define APPLICATION_HANDLER_MOCK_HPP

#include "i_application_handler.hpp"

#include "gmock/gmock.h"

namespace ExecutionManager
{

class ApplicationHandlerMock : public IApplicationHandler
{
public:
  MOCK_METHOD(void, startProcess, (const std::string& application));
  MOCK_METHOD(void, killProcess, (const std::string& processId));
};

} // namespace ExecutionManager

#endif // APPLICATION_HANDLER_MOCK_HPP
