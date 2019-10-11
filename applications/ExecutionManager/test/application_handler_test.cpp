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

TEST_F(ApplicationHandlerTest, ShouldStartChildProcess)                           
{
  const int expectedValue = 0;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo;

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execv(_,_)).WillOnce(Return(expectedValue));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  ASSERT_EQ(appHandler.startProcess(pinfo), expectedValue);
}

TEST_F(ApplicationHandlerTest, ShouldThrowExeptionIfFailedToCreateProcess)                              
{
  const int execvRes = 1;
  const int childProcessId = 0;

  ExecutionManager::ProcessInfo pinfo;

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execv(_,_)).WillOnce(Return(execvRes));

	ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};

  ASSERT_THROW(appHandler.startProcess(pinfo), runtime_error);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToKillProcess)
{
  pid_t processId = 5;

  EXPECT_CALL(*m_iosmock, kill(_, _));

	ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};

  appHandler.killProcess(processId);
}

ACTION_P(CheckProcessName, fullProcName)
{
  ASSERT_STREQ(fullProcName.c_str(), arg0);
}

ACTION_P(CheckArg, argData)
{
  ASSERT_STREQ(argData.second, arg1[argData.first]);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToGetDataLongForm)
{
  const int execvRes = 0;
  const int childProcessId = 0;

  ExecutionManager::StartupOption suoption = {ExecutionManager::StartupOptionKindEnum::commandLineLongForm, "abc", "def"};
  
  ExecutionManager::ProcessInfo pinfo = {" ", " ", {suoption}};
  std::string fullProcName = APPLICATIONS_PATH + pinfo.createRelativePath();
  const std::string cmdOption = "--" + suoption.optionName + "=" + suoption.optionArg;

  // Pairs containing argument info, first : position in arguments list, second : value.
  std::pair<int, const char*> procNameArg {0, pinfo.processName.c_str()};
  std::pair<int, const char*> suOptionArg {1, cmdOption.c_str()};
  std::pair<int, const char*> nullTerminatingArg {2, nullptr};

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execv(_, _))
    .WillOnce(DoAll(CheckProcessName(fullProcName),
  CheckArg(procNameArg),
  CheckArg(suOptionArg),
  CheckArg(nullTerminatingArg),
  Return(execvRes)));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  ASSERT_EQ(appHandler.startProcess(pinfo), childProcessId);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToGetDataShortForm)
{
  const int execvRes = 0;
  const int childProcessId = 0;

  ExecutionManager::StartupOption suoption = {ExecutionManager::StartupOptionKindEnum::commandLineShortForm, "abc", "def"};
  
  ExecutionManager::ProcessInfo pinfo = {" ", " ", {suoption}};
  std::string fullProcName = APPLICATIONS_PATH + pinfo.createRelativePath();
  const std::string cmdOption = "-" + suoption.optionName + " " + suoption.optionArg;

  // Pairs containing argument info, first : position in arguments list, second : value.
  std::pair<int, const char*> procNameArg {0, pinfo.processName.c_str()};
  std::pair<int, const char*> suOptionArg {1, cmdOption.c_str()};
  std::pair<int, const char*> nullTerminatingArg {2, nullptr};

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execv(_, _))
    .WillOnce(DoAll(CheckProcessName(fullProcName),
  CheckArg(procNameArg),
  CheckArg(suOptionArg),
  CheckArg(nullTerminatingArg),
  Return(execvRes)));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  ASSERT_EQ(appHandler.startProcess(pinfo), childProcessId);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToGetDataSimpleForm)
{
  const int execvRes = 0;
  const int childProcessId = 0;

  ExecutionManager::StartupOption suoption = {ExecutionManager::StartupOptionKindEnum::commandLineSimpleForm, "abc", "def"};
  
  ExecutionManager::ProcessInfo pinfo = {" ", " ", {suoption}};
  std::string fullProcName = APPLICATIONS_PATH + pinfo.createRelativePath();
  const std::string cmdOption = suoption.optionName;

  // Pairs containing argument info, first : position in arguments list, second : value.
  std::pair<int, const char*> procNameArg {0, pinfo.processName.c_str()};
  std::pair<int, const char*> suOptionArg {1, cmdOption.c_str()};
  std::pair<int, const char*> nullTerminatingArg {2, nullptr};

  EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
  EXPECT_CALL(*m_iosmock, execv(_, _))
    .WillOnce(DoAll(CheckProcessName(fullProcName),
  CheckArg(procNameArg),
  CheckArg(suOptionArg),
  CheckArg(nullTerminatingArg),
  Return(execvRes)));

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  ASSERT_EQ(appHandler.startProcess(pinfo), childProcessId);
}


}
