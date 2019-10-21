#include "adaptive_app.hpp"
#include "state.hpp"

#include <iostream>
#include <atomic>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

using ApplicationState = api::ApplicationStateClient::ApplicationState;

class StateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& app),(const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& app),(const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& app),(const));
};

class StateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

class ApplicationStateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

class ComponentClientMock : public api::IComponentClientWrapper
{
public:
    MOCK_METHOD(api::ComponentClientReturnType, SetStateUpdateHandler,
                (std::function<void(api::ComponentState const&)> f), (noexcept));

    MOCK_METHOD(api::ComponentClientReturnType, GetComponentState,
                (api::ComponentState& state), (noexcept));

    MOCK_METHOD(void, ConfirmComponentState,
                (api::ComponentState state, api::ComponentClientReturnType status), (noexcept));
};

class StateTest : public ::testing::Test
{
protected:
    std::unique_ptr<ApplicationStateClientMock> stateClientMock = std::make_unique<ApplicationStateClientMock>();
    std::unique_ptr<ComponentClientMock> componentClientMock = std::make_unique<ComponentClientMock>();
    std::unique_ptr<StateFactoryMock> factoryMock = std::make_unique<StateFactoryMock>();
    StateFactory factory;
    api::ComponentState emptyState;

    void setExpectationsForGetCompState(const api::ComponentState& initialState,
                                        api::ComponentState& expectedState,
                                        const api::ComponentClientReturnType& result,
                                        const int times = 1);
    void setExpectationsForConfirmCompState(const api::ComponentState& expectedState,
                                            api::ComponentClientReturnType result);
};


void StateTest::setExpectationsForGetCompState(const api::ComponentState& initialState,
                                               api::ComponentState& expectedState,
                                               const api::ComponentClientReturnType& result,
                                               const int times)
{
    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(initialState))).Times(times)
            .WillRepeatedly(
                DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(result))
                      );
}

void StateTest::setExpectationsForConfirmCompState(const api::ComponentState& expectedState,
                                        const api::ComponentClientReturnType result)
{
    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    result)
                )
            .WillOnce(Return());
}

TEST_F(StateTest, shouldReportStateWhenInitEntered)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(ApplicationState::K_INITIALIZING));//.WillOnce(Return());

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createInit(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmKOnWhenRunEntered)
{
    api::ComponentState expectedState = api::ComponentStateKOn;

    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kSuccess);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kSuccess);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmKOffWhenRunEntered)
{
    api::ComponentState expectedState = api::ComponentStateKOff;

    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kSuccess);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kSuccess);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmInvalidStateWhenRunEntered)
{
    api::ComponentState expectedState = "invalidState";

    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kSuccess);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kInvalid);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmGeneralErrorWhenRunEntered)
{
    api::ComponentState expectedState = "invalidState";

    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kGeneralError);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kGeneralError);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOnWhenRunEntered)
{
    api::ComponentState expectedState = api::ComponentStateKOn;

    constexpr int getCompStateTimes = 2;
    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kSuccess, getCompStateTimes);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kSuccess);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kUnchanged);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldConfirmStateUnchangedKOffWhenRunEntered)
{
    api::ComponentState expectedState = api::ComponentStateKOff;

    constexpr int getCompStateTimes = 2;
    setExpectationsForGetCompState(emptyState, expectedState, api::ComponentClientReturnType::kSuccess, getCompStateTimes);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kSuccess);
    setExpectationsForConfirmCompState(expectedState, api::ComponentClientReturnType::kUnchanged);

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createRun(*appMock);
    state->enter();
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldReportRunningStateWhenInitLeaved)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING))
            .WillOnce(Return());

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createInit(*appMock);
    state->leave();

    delete appMock;
}

TEST_F(StateTest, shouldReportShutdownStateWhenShurDownEntered)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN))
            .WillOnce(Return());

    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createShutDown(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, shouldPerformDefaultActionWhenShutdownLeaved)
{
    AdaptiveApp* appMock = new AdaptiveApp{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    auto state = factory.createShutDown(*appMock);
    state->leave();

    delete appMock;
}

