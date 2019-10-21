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

using namespace testing;

using ApplicationState = api::ApplicationStateClient::ApplicationState;

class StateTest : public ::testing::Test
{
protected:
    std::unique_ptr<api::AppStateClientMock> stateClientMock = std::make_unique<StrictMock<api::AppStateClientMock>>();
    std::unique_ptr<api::ComponentClientMock> componentClientMock = std::make_unique<StrictMock<api::ComponentClientMock>>();
    std::unique_ptr<api::StateFactoryMock> factoryMock = std::make_unique<StrictMock<api::StateFactoryMock>>();
    StateFactory factory;
    api::ComponentState emptyState;

    api::AppStateClientMock* stateClientMockPtr = stateClientMock.get();
    api::ComponentClientMock* componentClientMockPtr = componentClientMock.get();

    AdaptiveApp appMock{std::move(factoryMock),
                std::move(stateClientMock),
                std::move(componentClientMock)};

    const api::ComponentState expectedStateKOn = api::ComponentStateKOn;
    const api::ComponentState expectedStateKOff = api::ComponentStateKOff;
    const api::ComponentState expectedStateInvalid = "invalidState";

    void expectGetComponentState(const api::ComponentState& initialState,
                                 const api::ComponentState &expectedState,
                                 const api::ComponentClientReturnType& result);
    void expectConfirmComponentState(const api::ComponentState& expectedState,
                                     api::ComponentClientReturnType result);
};


void StateTest::expectGetComponentState(const api::ComponentState& initialState,
                                        const api::ComponentState& expectedState,
                                        const api::ComponentClientReturnType& result)
{
    EXPECT_CALL(*componentClientMockPtr, GetComponentState(Eq(initialState)))
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
    EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationState::K_INITIALIZING));//.WillOnce(Return());

    auto state = factory.createInit(appMock);
    state->enter();
}

TEST_F(StateTest, shouldConfirmKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    state->enter();
}

TEST_F(StateTest, shouldConfirmKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();
}

TEST_F(StateTest, shouldConfirmInvalidStateWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateInvalid, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kInvalid);
    state->enter();
}

TEST_F(StateTest, shouldConfirmGeneralErrorWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectGetComponentState(emptyState, expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(emptyState, expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kUnchanged);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(emptyState, expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kUnchanged);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateChangedFromKOnToKOffWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(emptyState, expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();
}

TEST_F(StateTest, shouldConfirmStateChangedFromKOffToKOnWhenRunEntered)
{
    auto state = factory.createRun(appMock);

    expectGetComponentState(emptyState, expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    state->enter();

    expectGetComponentState(emptyState, expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
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

