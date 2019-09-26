#include "msm_state_machine.hpp"
#include <mocks.hpp>

#include "gtest/gtest.h"

using namespace MSM;
using namespace testing;

class MsmStateMachineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        appStateClientMock = std::make_unique<AppStateClientMock>();
        factoryMock = std::make_unique<IStateFactoryMock>();
        machineStateClientMock = std::make_unique<MachineStateClientMock>();
    }

    void TearDown() override
    {
        delete msmMock;
    }
    
   std::unique_ptr<AppStateClientMock> appStateClientMock{nullptr};
   std::unique_ptr<IStateFactoryMock> factoryMock{nullptr};
   std::unique_ptr<MachineStateClientMock> machineStateClientMock{nullptr};
   MsmStateFactory factory;
   IMachineStateManagerMock* msmMock;
};

TEST_F(MsmStateMachineTest, ShouldInitCallEnter)
{
    EXPECT_CALL(*machineStateClientMock, Register(_,_)).WillOnce(Return(StateError::K_SUCCESS));
    
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};
    
    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, UnsuccessfulRegistration)
{
    EXPECT_CALL(*machineStateClientMock, Register(_,_)).WillOnce(Return(StateError::K_INVALID_STATE));
    
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};
    
    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->enter();
}


TEST_F(MsmStateMachineTest, ShouldRunCallEnter)
{
    EXPECT_CALL(*machineStateClientMock, SetMachineState(_,_))
        .Times(3)
        .WillRepeatedly(Return(StateError::K_SUCCESS));

    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    std::unique_ptr<::Run> state = std::make_unique<::Run>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldInitCallLeave)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallEnter)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallLeave)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldCreateInit)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    std::unique_ptr<api::IState> expectedState = std::make_unique<Init>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createInit(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateRun)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    std::unique_ptr<api::IState> expectedState = std::make_unique<::Run>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createRun(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateTerminate)
{
    msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                           std::move(appStateClientMock),
                                           std::move(machineStateClientMock)};

    std::unique_ptr<api::IState> expectedState = std::make_unique<ShutDown>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createShutDown(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}