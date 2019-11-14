#include "application_handler.hpp"
#include <mocks/os_interface_mock.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

namespace
{  

  ACTION_P(CheckProcessName, fullProcName)
  {
    ASSERT_STREQ("systemctl", arg0);
  }

  ACTION_P(CheckArg, argData)
  {
    ASSERT_STREQ(argData.second, arg1[argData.first]);
  }
}

class ApplicationHandlerTest : public Test
{
protected:
  void setupArgumentsCheck
  (const std::string& serviceName, const std::string& action)
  {
    static const std::pair<int, const char*> procNameArg {0, SYSTEMCTL.c_str()};
    static const std::pair<int, const char*> userArg{1, "--user"};
    std::pair<int, const char*> systemctlAction {2, action.c_str()};
    std::pair<int, const char*> suOptionArg {3, serviceName.c_str()};
    static const std::pair<int, const char*> nullTerminatingArg {4, nullptr};

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
    EXPECT_CALL(*m_iosmock, execvp(_, _))
      .WillOnce(DoAll(CheckProcessName(SYSTEMCTL),
                      CheckArg(procNameArg),
                      CheckArg(userArg),
                      CheckArg(systemctlAction),
                      CheckArg(suOptionArg),
                      CheckArg(nullTerminatingArg),
                      Return(execvRes)));
  }

  const int childProcessId = 0;
  const int execvRes = 0;
  std::unique_ptr<OSInterfaceMock> m_iosmock = 
    std::make_unique<StrictMock<OSInterfaceMock>>();
  const std::string processName = "process";
  const std::string serviceName = processName + SERVICE_EXTENSION;
};

TEST_F(ApplicationHandlerTest, ShouldFailToStartProcessWhenForkFailed)
{
  EXPECT_CALL(*m_iosmock, fork())
    .WillOnce(SetErrnoAndReturn(EAGAIN, -1));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(processName);
}

TEST_F(ApplicationHandlerTest, ShouldFailToStartProcessWhenExecvpFailed)
{
  EXPECT_CALL(*m_iosmock, fork())
    .WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_,_))
    .WillOnce(SetErrnoAndReturn(EAGAIN, -1));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(processName);
}


TEST_F(ApplicationHandlerTest, ShouldSucceedToStartService)
{
  setupArgumentsCheck(serviceName, SYSTEMCTL_START);

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(processName);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToStopService)
{
  setupArgumentsCheck(serviceName, SYSTEMCTL_STOP);

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.killProcess(processName);
}