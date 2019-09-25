#include "application_handler.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>

using namespace std;
using namespace testing;

namespace ApplicationHandlerTest
{  
class OSInterfaceMock : public IOsInterface
{
public:
    OSInterfaceMock(){};

    MOCK_METHOD(pid_t, fork, ());
    MOCK_METHOD(int, execv, (const char* path, char* argv[]));
    MOCK_METHOD(int, kill, (pid_t procId, int signal));
};

class ApplicationHandlerTest : public ::testing::Test 
{
protected:
    unique_ptr<OSInterfaceMock> m_iosmock = make_unique<OSInterfaceMock>();
};

TEST_F(ApplicationHandlerTest, ShouldSucceedToStartProcess)
{
    const int expectedValue = 1; 
    
    ExecutionManager::ProcessInfo pinfo;

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(expectedValue));

	ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};

    ASSERT_EQ(ah.startProcess(pinfo), expectedValue);
}

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

    suoption.optionArg = "abc";
    suoption.optionKind = ExecutionManager::StartupOptionKindEnum::commandLineLongForm;
    suoption.optionName = "def";
    pinfo.applicationName = " ";
    pinfo.processName = " ";
    pinfo.startOptions = {suoption};

    EXPECT_CALL(*m_iosmock, fork()).WillOnce(Return(childProcessId));
    EXPECT_CALL(*m_iosmock, execv(_,_)).WillOnce(Return(execvRes));  
	
    ExecutionManager::ApplicationHandler ah{std::move(m_iosmock)};         
    ASSERT_EQ(ah.startProcess(pinfo), childProcessId);
}
}
