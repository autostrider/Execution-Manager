#include "state.hpp"
#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/mean_calculator_mock.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

using ApplicationState = api::ApplicationStateClient::ApplicationState;

class StateTest : public ::testing::Test
{
protected:
    std::unique_ptr<api::AppStateClientMock> stateClientMock = std::make_unique<StrictMock<api::AppStateClientMock>>();
    std::unique_ptr<api::ComponentClientMock> componentClientMock = std::make_unique<StrictMock<api::ComponentClientMock>>();
    std::unique_ptr<api::StateFactoryMock> factoryMock = std::make_unique<StrictMock<api::StateFactoryMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();
    StateFactory factory;

    api::AppStateClientMock* stateClientMockPtr = stateClientMock.get();
    api::ComponentClientMock* componentClientMockPtr = componentClientMock.get();
    MeanCalculatorMock* meanCalculatorMockPtr = meanCalculatorMock.get();

    AdaptiveApp appMock{std::move(factoryMock),
                std::move(stateClientMock),
                std::move(componentClientMock),
                       std::move(meanCalculatorMock)};
};

TEST_F(StateTest, shouldReportStateWhenInitEntered)
{
    auto state = factory.createInit(appMock);
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationState::K_INITIALIZING));
    state->enter();
}

TEST_F(StateTest, shouldReportRunningStateWhenRunEntered)
{
    auto state = factory.createRun(appMock);
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING));
    state->enter();
}

TEST_F(StateTest, shouldCalculateMeanWhenPerforemActionCalled)
{
    auto state = factory.createRun(appMock);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    state->performAction();
}

TEST_F(StateTest, shouldReportShutdownStateWhenShutDownEntered)
{
    auto state = factory.createShutDown(appMock);
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN));
    state->enter();
}

TEST_F(StateTest, shouldDoNothingWhenShutdownLeft)
{
    auto state = factory.createShutDown(appMock);
    state->leave();
}

TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForInitState)
{
    auto state = factory.createInit(appMock);
    state->performAction();
}
TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForSuspendState)
{
    auto state = factory.createSuspend(appMock);
    state->performAction();
}

TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForShutdownState)
{
    auto state = factory.createShutDown(appMock);
    state->performAction();
}

TEST_F(StateTest, shouldDoNothingWhenSuspendEntered)
{
    auto state = factory.createSuspend(appMock);
    state->leave();
}
TEST_F(StateTest, shouldDoNothingWhenSuspendLeft)
{
    auto state = factory.createSuspend(appMock);
    state->enter();
}
