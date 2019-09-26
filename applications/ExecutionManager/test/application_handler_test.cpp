#include "application_handler.hpp"
#include <mocks.hpp>

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace ApplicationHandlerTest
{  
class ApplicationHandlerTest : public ::testing::Test 
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

    ExecutionManager::ProcessInfo pinfo;
    ExecutionManager::StartupOption suoption;

    suoption = {ExecutionManager::StartupOptionKindEnum::commandLineLongForm, "abc", "def"};
    pinfo = {" ", " ", {suoption}};

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
    EXPECT_CALL(*m_iosmock, execv(_,_)).WillOnce(Return(execvRes));  
	
    ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};         
    ASSERT_EQ(ah.startProcess(pinfo), childProcessId);
}
}
