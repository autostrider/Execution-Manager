#include <application_handler.hpp>

#include <mocks/os_interface_mock.hpp>

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace ApplicationHandlerTest
{  

  ACTION_P(CheckProcessName, fullProcName)
  {
    ASSERT_STREQ("systemctl", arg0);
  }

  ACTION_P(CheckArg, argData)
  {
    ASSERT_STREQ(argData.second, arg1[argData.first]);
  }

  ACTION_TEMPLATE(SetArgNPointeeTo, HAS_1_TEMPLATE_PARAMS(unsigned, uIndex),
                  AND_2_VALUE_PARAMS(pData, uiDataSize))
  {
      std::memcpy(std::get<uIndex>(args), pData, uiDataSize);
  }

class ApplicationHandlerTest : public ::testing::Test
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
  OSInterfaceMock* m_iosMockPtr = m_iosmock.get();
  const std::string processName = "process";
  const std::string serviceName = processName + SERVICE_EXTENSION;
};

TEST_F(ApplicationHandlerTest, ShouldFailToStartProcessWhenForkFailed)
{
  constexpr int forkRes = -1;

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
  static const std::string start{"start"};
  setupArgumentsCheck(serviceName, start);

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.startProcess(processName);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToStopService)
{
  static const std::string stop{"stop"};
  setupArgumentsCheck(serviceName, stop);

  ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
  appHandler.killProcess(processName);
}

TEST_F(ApplicationHandlerTest, ShouldReturnTrueIfAppIsAlive)
{
    const std::string someRealSystemApp("bash");
    char data[] = "1234";
    size_t data_size = sizeof (data);
    auto someFile = std::make_unique<FILE>();
    EXPECT_CALL(*m_iosMockPtr, popen(_, _)).WillOnce(Return(someFile.get()));
    EXPECT_CALL(*m_iosMockPtr, fread(_, _, _, _))
      .WillOnce(SetArgNPointeeTo<0>(std::begin(data), data_size));

    EXPECT_CALL(*m_iosMockPtr, pclose(_));
    ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
    ASSERT_TRUE(appHandler.isActiveProcess(someRealSystemApp));
}

TEST_F(ApplicationHandlerTest, ShouldReturnFalseIfAppIsNotAlive)
{
    const std::string someRealSystemApp("bash");
    char data[] = "qwerty";
    size_t data_size = sizeof (data);
    auto someFile = std::make_unique<FILE>();
    EXPECT_CALL(*m_iosMockPtr, popen(_, _)).WillOnce(Return(someFile.get()));
    EXPECT_CALL(*m_iosMockPtr, fread(_, _, _, _))
      .WillOnce(SetArgNPointeeTo<0>(std::begin(data), data_size));
    EXPECT_CALL(*m_iosMockPtr, pclose(_));
    ExecutionManager::ApplicationHandler appHandler{std::move(m_iosmock)};
    ASSERT_FALSE(appHandler.isActiveProcess(someRealSystemApp));
}

}