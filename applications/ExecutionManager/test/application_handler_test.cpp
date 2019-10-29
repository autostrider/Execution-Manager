#include "application_handler.hpp"
#include <mocks/os_interface_mock.hpp>

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace
{

}

namespace ApplicationHandlerTest
{  
class ApplicationHandlerTest :
  public ::testing::TestWithParam<ExecutionManager::StartupOptionKindEnum>
{
protected:
  unique_ptr<OSInterfaceMock> m_iosmock = make_unique<OSInterfaceMock>();
};

TEST_F(ApplicationHandlerTest, ShouldStartChildProcess)
{
  const int expectedValue = 0;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo;

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_,_)).WillOnce(Return(expectedValue));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  ASSERT_EQ(appHandler.startProcess(pinfo.processName), expectedValue);
}

TEST_F(ApplicationHandlerTest, ShouldThrowExeptionIfFailedToCreateProcess)
{
  const int execvRes = 1;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo;

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execvp(_,_)).WillOnce(Return(execvRes));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};

  ASSERT_THROW(appHandler.startProcess(pinfo.processName), runtime_error);
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


  ExecutionManager::ProcessInfo pinfo = {" ", " ", {}};
  const std::string serviceName = pinfo.processName + SERVICE_EXTENSION;
  constexpr auto systemctl {"systemctl"};

  // Pairs containing argument info, first : position in arguments list, second : value.
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
  ASSERT_EQ(appHandler.startProcess(pinfo.processName), childProcessId);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToStopService)
{
  const int execvRes = 0;
  const int childProcessId = 0;


  ExecutionManager::ProcessInfo pinfo = {" ", " ", {}};
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
