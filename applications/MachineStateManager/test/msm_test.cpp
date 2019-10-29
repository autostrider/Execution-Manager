#include "machine_state_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/machine_state_client_mock.hpp>
#include <mocks/i_state_factory_mock.hpp>
#include <mocks/i_state_mock.hpp>

#include "gtest/gtest.h"

using namespace MSM;
using namespace testing;
using namespace api;

class MsmTest : public ::testing::Test
{
protected:
   std::unique_ptr<StateFactoryMock> factoryMock{nullptr};
   std::unique_ptr<AppStateClientMock> appStateClientMock{nullptr};
   std::unique_ptr<MachineStateClientMock> machineStateClientMock{nullptr};

   std::unique_ptr<NiceMock<StateMock>> stateInitMock{nullptr};
   std::unique_ptr<NiceMock<StateMock>> stateRunMock{nullptr};
   std::unique_ptr<NiceMock<StateMock>> stateTermMock{nullptr};

    void SetUp() override
    {
        appStateClientMock = std::make_unique<AppStateClientMock>();
        factoryMock = std::make_unique<StateFactoryMock>();
        machineStateClientMock = std::make_unique<MachineStateClientMock>();

        stateInitMock = std::make_unique<NiceMock<StateMock>>();
        stateRunMock = std::make_unique<NiceMock<StateMock>>();
        stateTermMock = std::make_unique<NiceMock<StateMock>>();

        EXPECT_CALL(*factoryMock, createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    }
};

class ReportingStateTest : public MsmTest
{
protected:
    void SetUp() override
    {
        appStateClientMock = std::make_unique<AppStateClientMock>();
    }
};

TEST_F(MsmTest, ShouldTransitToInitState)
{
     MachineStateManager msm{std::move(factoryMock), nullptr, nullptr};
     msm.init();
}

TEST_F(MsmTest, ShouldTransitToRunState)
{
    EXPECT_CALL(*factoryMock,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

     MachineStateManager msm{std::move(factoryMock), nullptr, nullptr};
     msm.init();
     msm.run();
}

TEST_F(MsmTest, ShouldTransitToTerminateState)
{
    EXPECT_CALL(*factoryMock,
                createShutDown((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

     MachineStateManager msm{std::move(factoryMock), nullptr, nullptr};
     msm.init();
     msm.terminate();
}

TEST_F(ReportingStateTest, ShouldReportCurrentState)
{
    EXPECT_CALL(*appStateClientMock, ReportApplicationState(_)).WillOnce(Return());

    MachineStateManager msm{nullptr, std::move(appStateClientMock), nullptr};
    msm.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
}
