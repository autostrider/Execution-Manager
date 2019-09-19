#include "msm_state_machine.hpp"
#include "machine_state_manager.hpp"

#include <atomic>
#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace MachineStateManager;

using StateError = api::MachineStateClient::StateError;

class IMachineStateManagerMock : public MachineStateManager
{
    public:
        IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<api::IApplicationStateClientWrapper> client);

        MOCK_METHOD(void, init, ());
        MOCK_METHOD(void, run, ());
        MOCK_METHOD(void, terminate, ());

        MOCK_METHOD(void, reportApplicationState,
                    (api::ApplicationStateClient::ApplicationState state));
        MOCK_METHOD(void, transitToNextState, (IMachineStateManagerMock::FactoryFunc nextState));
};

IMachineStateManagerMock::IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
                                   std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    MachineStateManager(std::move(factory), std::move(client))
{}

class IStateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp &msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp &msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp &msm), (const));
};

class IStateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

class StateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD1(ReportApplicationState, void(ApplicationStateManagement::ApplicationState state));
};

class MsmStateMachineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        stateClientMock = std::make_unique<StateClientMock>();
        factoryMock = std::make_unique<IStateFactoryMock>();
        msmMock = new IMachineStateManagerMock{std::move(factoryMock),
                                               std::move(stateClientMock)};
    }

    void TearDown() override
    {
        delete msmMock;
    }
    
   std::unique_ptr<StateClientMock> stateClientMock{nullptr};
   std::unique_ptr<IStateFactoryMock> factoryMock{nullptr};
   IMachineStateManagerMock* msmMock{nullptr};
   MsmStateFactory factory;
};

TEST_F(MsmStateMachineTest, ShouldInitCallEnter)
{
    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldRunCallEnter)
{
    std::unique_ptr<::Run> state = std::make_unique<::Run>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldInitCallLeave)
{
    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallEnter)
{
    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallLeave)
{
    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldCreateInit)
{
    std::unique_ptr<api::IState> expectedState = std::make_unique<Init>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createInit(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateRun)
{
    std::unique_ptr<api::IState> expectedState = std::make_unique<::Run>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createRun(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateTerminate)
{
    std::unique_ptr<api::IState> expectedState = std::make_unique<ShutDown>(*msmMock);
    std::unique_ptr<api::IState> createdState = factory.createShutDown(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}
