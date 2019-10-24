#include "adaptive_app.hpp"
#include "state.hpp"

#include <iostream>
#include <atomic>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/mean_calculator_mock.hpp"

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

    const api::ComponentState expectedStateKOn = api::ComponentStateKOn;
    const api::ComponentState expectedStateKOff = api::ComponentStateKOff;
    const api::ComponentState expectedStateInvalid = "invalidState";
    const double mu = 10;
    const double sigma = 0.5;

    void expectGetComponentState(const api::ComponentState &expectedState,
                                 const api::ComponentClientReturnType& result);
    void expectConfirmComponentState(const api::ComponentState& expectedState,
                                     api::ComponentClientReturnType result);
};


void StateTest::expectGetComponentState(const api::ComponentState& expectedState,
                                        const api::ComponentClientReturnType& result)
{
    EXPECT_CALL(*componentClientMockPtr, GetComponentState(_))
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(expectedState),
                    Return(result))
                );
}

void StateTest::expectConfirmComponentState(const api::ComponentState& expectedState,
                                            const api::ComponentClientReturnType result)
{
    EXPECT_CALL(*componentClientMockPtr,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    result)
                );
}

TEST_F(StateTest, shouldReportStateWhenInitEntered)
{
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationState::K_INITIALIZING));

    auto state = factory.createInit(appMock);
    state->enter();
}

TEST_F(StateTest, shouldConfirmKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());

    state->enter();
}

TEST_F(StateTest, shouldConfirmKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();
}

TEST_F(StateTest, shouldConfirmInvalidStateWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kInvalid);
    state->enter();
}

TEST_F(StateTest, shouldConfirmGeneralErrorWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    state->enter();

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kUnchanged);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kUnchanged);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateChangedFromKOnToKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    state->enter();

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateChangedFromKOffToKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    state->enter();
}

TEST_F(StateTest, shouldReportRunningStateWhenInitLeft)
{
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING));

    auto state = factory.createInit(appMock);
    state->leave();
}

TEST_F(StateTest, shouldReportShutdownStateWhenShutDownEntered)
{
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN));

    auto state = factory.createShutDown(appMock);
    state->enter();
}

TEST_F(StateTest, shouldDoNothingWhenShutdownLeft)
{
    auto state = factory.createShutDown(appMock);
    state->leave();
}


