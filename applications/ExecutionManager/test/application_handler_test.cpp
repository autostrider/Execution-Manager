#include "application_handler.hpp"
#include <mocks.hpp>

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

  ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};
  ASSERT_EQ(ah.startProcess(pinfo), expectedValue);
}

TEST_F(ApplicationHandlerTest, ShouldThrowExeptionIfFailedToCreateProcess)                              
{
    const int execvRes = 1;
    const int childProcessId = 0;

    ExecutionManager::ProcessInfo pinfo;

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
    EXPECT_CALL(*m_iosmock, execv(_,_)).WillOnce(Return(execvRes));

	ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};

    ASSERT_THROW(ah.startProcess(pinfo), runtime_error);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToKillProcess)
{
    pid_t processId = 5;

    EXPECT_CALL(*m_iosmock, kill(_, _));

	ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};

    ah.killProcess(processId);
}

TEST_F(ApplicationHandlerTest, ShouldSucceedToGetData)
{
    const int execvRes = 0;
    const int childProcessId = 0;

  std::vector<ExecutionManager::StartupOption> suoptions =
    { {ExecutionManager::StartupOptionKindEnum::commandLineLongForm,
     "abc", "def"},
      {ExecutionManager::StartupOptionKindEnum::commandLineShortForm,
        "abc", "def"},
      {ExecutionManager::StartupOptionKindEnum::commandLineSimpleForm,
        "abc", "def"}};
  ExecutionManager::ProcessInfo pinfo =
    {" ", " ", suoptions};

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
    EXPECT_CALL(*m_iosmock, execv(_, _))
      .Times(1)
      .WillOnce(DoAll(Invoke([&](const char* procName,  char** listOfArguments)
                             {
                               std::string fullProcName = APPLICATIONS_PATH + pinfo.createRelativePath();

                               ASSERT_STREQ(fullProcName.c_str(), procName);
                               ASSERT_STREQ(pinfo.processName.c_str(), listOfArguments[0]);

                               for (auto suoption = suoptions.cbegin();
                                    suoption != suoptions.cend();
                                    suoption++)
                               {
                                 const std::string cmdOption = suoption->makeCommandLineOption();

                                 ASSERT_STREQ(cmdOption.c_str(),
                                   listOfArguments[1 + (suoption - suoptions.cbegin())]);
                               }
                               ASSERT_EQ(nullptr,
                                        listOfArguments[1 + suoptions.size()]);
                             }),
                      Return(execvRes)));

  ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};
    ASSERT_EQ(ah.startProcess(pinfo), childProcessId);
}
}
