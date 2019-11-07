#include "application_handler.hpp"
#include <mocks/os_interface_mock.hpp>

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace ApplicationHandlerTest
{  
class ApplicationHandlerTest :
  public ::testing::TestWithParam<ExecutionManager::StartupOptionKindEnum>
{
protected:
  unique_ptr<OSInterfaceMock> m_iosmock = make_unique<OSInterfaceMock>();
};

TEST_F(ApplicationHandlerTest, ShouldFailToStartProcessWhenForkFailed)
{
  constexpr int forkRes = -1;
  ExecutionManager::ProcessInfo pinfo = {"app", "process", {}};
  const std::string serviceName = pinfo.processName + SERVICE_EXTENSION;
  constexpr auto systemctl {"systemctl"};

  EXPECT_CALL(*m_iosmock, fork())
    .WillOnce(SetErrnoAndReturn(EAGAIN, -1));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(pinfo.processName);
}

TEST_F(ApplicationHandlerTest, ShouldFailToStartProcessWhenExecvpFailed)
{
  constexpr int execvpResult = -1;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo = {"app", "process", {}};
  const std::string serviceName = pinfo.processName + SERVICE_EXTENSION;
  constexpr auto systemctl {"systemctl"};

  EXPECT_CALL(*m_iosmock, fork())
    .WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_,_))
    .WillOnce(SetErrnoAndReturn(EAGAIN, -1));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(pinfo.processName);
}

ACTION_P(CheckProcessName, fullProcName)
{
  ASSERT_STREQ("systemctl", arg0);
}

ACTION_P(CheckArg, argData)
{
  ASSERT_STREQ(argData.second, arg1[argData.first]);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToStartService)
{
  const int execvRes = 0;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo = {"app", "process", {}};
  const std::string serviceName = pinfo.processName + SERVICE_EXTENSION;
  constexpr auto systemctl {"systemctl"};

  // Pairs containing argument info, first : position in arguments list, 
  // second : value.
  std::pair<int, const char*> procNameArg {0, systemctl};
  std::pair<int, const char*> userArg{1, "--user"};
  std::pair<int, const char*> systemctlAction {2, "start"};
  std::pair<int, const char*> suOptionArg {3, serviceName.c_str()};
  std::pair<int, const char*> nullTerminatingArg {4, nullptr};

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_, _))
    .WillOnce(DoAll(CheckProcessName(systemctl),
  CheckArg(procNameArg),
  CheckArg(userArg),
  CheckArg(systemctlAction),
  CheckArg(suOptionArg),
  CheckArg(nullTerminatingArg),
  Return(execvRes)));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(pinfo.processName);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToStopService)
{
  const int execvRes = 0;
  const int childProcessId = 0;


  ExecutionManager::ProcessInfo pinfo = {"app", "process", {}};
  const std::string serviceName = pinfo.processName + SERVICE_EXTENSION;
  constexpr auto systemctl {"systemctl"};

  // Pairs containing argument info, first : position in arguments list, second : value.
  std::pair<int, const char*> procNameArg {0, systemctl};
  std::pair<int, const char*> userArg{1, "--user"};
  std::pair<int, const char*> systemctlAction {2, "stop"};
  std::pair<int, const char*> suOptionArg {3, serviceName.c_str()};
  std::pair<int, const char*> nullTerminatingArg {4, nullptr};

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_, _))
      .WillOnce(DoAll(CheckProcessName(systemctl),
    CheckArg(procNameArg),
    CheckArg(userArg),
    CheckArg(systemctlAction),
    CheckArg(suOptionArg),
    CheckArg(nullTerminatingArg),
    Return(execvRes)));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.killProcess(pinfo.processName);
}


}
