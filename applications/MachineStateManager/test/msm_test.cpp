#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

#include <atomic>
#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace MSM;
/*
class IStateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp &msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp &msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp &msm), (const));
};

class StateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

class IStateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

class MsmTest : public ::testing::Test
{
protected:
   std::unique_ptr<StateClientMock> stateClientMock{std::make_unique<StateClientMock>()};
   std::unique_ptr<IStateFactoryMock> factoryMock{std::make_unique<IStateFactoryMock>()};
   std::unique_ptr<NiceMock<IStateMock>> stateInitMock = std::make_unique<NiceMock<IStateMock>>();
   std::unique_ptr<NiceMock<IStateMock>> stateRunMock = std::make_unique<NiceMock<IStateMock>>();
   std::unique_ptr<NiceMock<IStateMock>> stateTermMock = std::make_unique<NiceMock<IStateMock>>();

    void SetUp() override
    {
        EXPECT_CALL(*factoryMock, createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    }
};

class ReportingStateTest : public MsmTest
{
protected:
    void SetUp() override {}
};

TEST_F(MsmTest, ShouldTransitToInitState)
{
     MachineStateManager msm{std::move(factoryMock), nullptr};
     msm.init();
}

TEST_F(MsmTest, ShouldTransitToRunState)
{
    EXPECT_CALL(*factoryMock,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

     MachineStateManager msm{std::move(factoryMock),
                     nullptr};
     msm.init();
     msm.run();
}

TEST_F(MsmTest, ShouldTransitToTerminateState)
{
    EXPECT_CALL(*factoryMock,
                createShutDown((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

     MachineStateManager msm{std::move(factoryMock),
                     nullptr};
     msm.init();
     msm.terminate();
}

TEST_F(ReportingStateTest, ShouldReportCurrentState)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(_)).WillOnce(Return());

    MachineStateManager msm{nullptr, std::move(stateClientMock)};
    msm.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
}
*/
