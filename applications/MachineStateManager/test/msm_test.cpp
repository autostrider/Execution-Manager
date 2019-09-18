#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

#include <atomic>
#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace MachineStateManager;

class IStateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp &msm));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp &msm));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp &msm));
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
};

TEST_F(MsmTest, Should_TransitToInitState)
{
    EXPECT_CALL(*factoryMock,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

     MachineStateManager::MachineStateManager msm{std::move(factoryMock), nullptr};
     msm.init();

}

TEST_F(MsmTest, Should_TransitToRunState)
{
    EXPECT_CALL(*factoryMock,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    EXPECT_CALL(*factoryMock,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

     MachineStateManager::MachineStateManager msm{std::move(factoryMock),
                     nullptr};
     msm.init();
     msm.run();
}

TEST_F(MsmTest, Should_TransitToTerminateState)
{
    EXPECT_CALL(*factoryMock,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*factoryMock,
                createShutDown((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

     MachineStateManager::MachineStateManager msm{std::move(factoryMock),
                     nullptr};
     msm.init();
     msm.terminate();
}

TEST_F(MsmTest, Should_ReportCurrentState)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(_)).WillOnce(Return());

    MachineStateManager::MachineStateManager msm{nullptr, std::move(stateClientMock)};
    msm.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
}
