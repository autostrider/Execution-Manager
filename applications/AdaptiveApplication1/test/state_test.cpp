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
class IAdaptiveAppMock : public AdaptiveApp
{
public:
    IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
                     std::unique_ptr<api::IApplicationStateClientWrapper> client,
                     std::unique_ptr<api::IComponentClientWrapper> compClient);

    MOCK_METHOD(void, init, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, terminate, ());

    MOCK_METHOD(void, reportApplicationState,
                (api::ApplicationStateClient::ApplicationState state));
    MOCK_METHOD(void, transitToNextState, (IAdaptiveApp::FactoryFunc nextState));
};

IAdaptiveAppMock::IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
                                   std::unique_ptr<api::IApplicationStateClientWrapper> client,
                                   std::unique_ptr<api::IComponentClientWrapper> compClient) :
    AdaptiveApp(std::move(factory), std::move(client), std::move(compClient))
{

}

class StateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        stateClientMock = std::make_unique<ApplicationStateClientMock>();
        componentClientMock = std::make_unique<ComponentClientMock>();
        factoryMock = std::make_unique<StateFactoryMock>();
    }

    std::unique_ptr<ApplicationStateClientMock> stateClientMock{nullptr};
    std::unique_ptr<ComponentClientMock> componentClientMock{nullptr};
    std::unique_ptr<StateFactoryMock> factoryMock{nullptr};
    StateFactory factory;
    api::ComponentState emptyState;
};


TEST_F(StateTest, Should_InitOnEnterReportState)
{
    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    EXPECT_CALL(*appMock, reportApplicationState(ApplicationState::K_INITIALIZING));//.WillOnce(Return());

    std::unique_ptr<api::IState> state = factory.createInit(*appMock);
    state->enter();
    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmKOn)
{
    api::ComponentState expectedState = api::ComponentStateKOn;

    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState)))
            .WillOnce(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kSuccess))
                      );
    EXPECT_CALL(*componentClientMock, ConfirmComponentState(expectedState, api::ComponentClientReturnType::kSuccess));

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmKOff)
{
    api::ComponentState expectedState = api::ComponentStateKOff;

    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState)))
            .WillOnce(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kSuccess))
                      );
    EXPECT_CALL(*componentClientMock, ConfirmComponentState(expectedState, api::ComponentClientReturnType::kSuccess));

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmInvalidState)
{
    api::ComponentState expectedState = "invalidState";
    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState)))
            .WillOnce(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kSuccess))
                      );
    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kInvalid)
                )
            .WillOnce(Return());

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmGeneralError)
{
    api::ComponentState expectedState = "invalidState";
    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState)))
            .WillOnce(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kGeneralError))
                      );
    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kGeneralError)
                )
            .WillOnce(Return());

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmStateUnchangedKOn)
{
    api::ComponentState expectedState = api::ComponentStateKOn;
    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState))).Times(2)
            .WillRepeatedly(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kSuccess))
                      );
    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kSuccess)
                )
            .WillOnce(Return());

    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kUnchanged)
                )
            .WillOnce(Return());

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();



    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_RunOnEnterConfirmStateUnchangedKOff)
{
    api::ComponentState expectedState = api::ComponentStateKOff;
    EXPECT_CALL(*componentClientMock, GetComponentState(Eq(emptyState))).Times(2)
            .WillRepeatedly(DoAll(
                          SetArgReferee<0>(expectedState),
                          Return(api::ComponentClientReturnType::kSuccess))
                      );
    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kSuccess)
                )
            .WillOnce(Return());

    EXPECT_CALL(*componentClientMock,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    api::ComponentClientReturnType::kUnchanged)
                )
            .WillOnce(Return());

    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createRun(*appMock);
    state->enter();
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_InitOnLeaveReportRunningState)
{
    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    EXPECT_CALL(*appMock, reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING))
            .WillOnce(Return());

    std::unique_ptr<api::IState> state = factory.createInit(*appMock);
    state->leave();

    delete appMock;
}

TEST_F(StateTest, Should_TerminateOnEnterReportShutdownState)
{
    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    EXPECT_CALL(*appMock, reportApplicationState(api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN))
            .WillOnce(Return());

    std::unique_ptr<api::IState> state = factory.createShutDown(*appMock);
    state->enter();

    delete appMock;
}

TEST_F(StateTest, Should_TerminateOnLeaveDefaultAction)
{
    IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
            std::move(stateClientMock),
            std::move(componentClientMock)};

    std::unique_ptr<api::IState> state = factory.createShutDown(*appMock);
    state->leave();

    delete appMock;
}
