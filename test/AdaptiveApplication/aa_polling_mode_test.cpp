#include <adaptive_app.hpp>

#include <i_state_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <app_state_client_mock.hpp>
#include <component_client_mock.hpp>
#include <mean_calculator_mock.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace api;
using namespace application_state;
using namespace component_client;
using namespace testing;

class AppPollingModeTest : public ::testing::Test
{
protected:
    std::unique_ptr<AppStateClientMock> stateClientMock{std::make_unique<StrictMock<AppStateClientMock>>()};

    std::unique_ptr<StateFactoryMock> factoryMock{std::make_unique<StrictMock<StateFactoryMock>>()};
    std::unique_ptr<StateMock> stateInitMock = std::make_unique<StrictMock<StateMock>>();
    std::unique_ptr<StateMock> stateRunMock = std::make_unique<StrictMock<StateMock>>();
    std::unique_ptr<StateMock> stateTermMock = std::make_unique<StrictMock<StateMock>>();
    std::unique_ptr<StateMock> stateSuspendMock = std::make_unique<StrictMock<StateMock>>();

    std::unique_ptr<ComponentClientMock> compStateMock = std::make_unique<StrictMock<ComponentClientMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    StateMock* stateInitMockPtr = stateInitMock.get();
    StateMock* stateRunMockPtr = stateRunMock.get();
    StateMock* stateSuspendMockPtr = stateSuspendMock.get();
    StateFactoryMock* factoryPtr = factoryMock.get();
    AppStateClientMock* stateClientMockPtr = stateClientMock.get();
    MeanCalculatorMock* meanCalculatorMockPtr = meanCalculatorMock.get();
    ComponentClientMock* compStateMockPtr = compStateMock.get();

    AdaptiveApp app{std::move(factoryMock), std::move(stateClientMock),
                std::move(compStateMock), std::move(meanCalculatorMock)};

    const ComponentState expectedStateKOn = COMPONENT_STATE_ON;
    const ComponentState expectedStateKOff = COMPONENT_STATE_OFF;
    const ComponentState expectedStateInvalid = "invalidState";

    void expectGetComponentState(const ComponentState &expectedState,
                                 const ComponentClientReturnType& result);
    void expectConfirmComponentState(const ComponentState& expectedState,
                                     ComponentClientReturnType result);
    void goToInit();
    void goToRunFromState(StateMock *oldStateMock);
    void expectGoToSuspendFromState(StateMock *oldStateMock);
    void performActionOnState(StateMock *stateMock);
    void resume();
};

void AppPollingModeTest::expectGetComponentState(const ComponentState& expectedState,
                                      const ComponentClientReturnType& result)
{
    EXPECT_CALL(*compStateMockPtr, GetComponentState(_))
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(expectedState),
                    Return(result))
                );
}

void AppPollingModeTest::expectConfirmComponentState(const ComponentState& expectedState,
                                          const ComponentClientReturnType result)
{
    EXPECT_CALL(*compStateMockPtr,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    result)
                );
}

void AppPollingModeTest::goToInit()
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    app.init();
}

void AppPollingModeTest::goToRunFromState(StateMock* oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*stateRunMock, enter());
    EXPECT_CALL(*factoryPtr,
                createRun(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));
    app.run();
}

void AppPollingModeTest::expectGoToSuspendFromState(StateMock *oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*factoryPtr, createSuspend(Ref(app))).WillOnce(Return(ByMove(std::move(stateSuspendMock))));
    EXPECT_CALL(*stateSuspendMockPtr, enter());
}

void AppPollingModeTest::performActionOnState(StateMock *stateMock)
{
    EXPECT_CALL(*stateMock, performAction());
    app.performAction();
}

void AppPollingModeTest::resume()
{
    {
        InSequence seq;

        stateRunMock = std::make_unique<StrictMock<StateMock>>();
        stateRunMockPtr = stateRunMock.get();

        expectGetComponentState(expectedStateKOn, ComponentClientReturnType::K_SUCCESS);
        EXPECT_CALL(*stateSuspendMockPtr, leave());
        EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
        EXPECT_CALL(*stateRunMockPtr, enter());
        expectConfirmComponentState(expectedStateKOn, ComponentClientReturnType::K_SUCCESS);

    }
}

TEST_F(AppPollingModeTest, Should_StayInKOnWhenPerformActionCalled)
{
    goToInit();
    expectGetComponentState(expectedStateKOn, ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, ComponentClientReturnType::K_UNCHANGED);
    performActionOnState(stateInitMockPtr);
}

TEST_F(AppPollingModeTest, shouldReportErrorWhenErrorOccuredWhileGettingCompState)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, ComponentClientReturnType::K_GENERAL_ERROR);
    expectConfirmComponentState(expectedStateKOn, ComponentClientReturnType::K_GENERAL_ERROR);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, ComponentClientReturnType::K_UNCHANGED);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenInvalidStateReceived)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateInvalid, ComponentClientReturnType::K_INVALID);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenGetComponentStateFailed)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, ComponentClientReturnType::K_GENERAL_ERROR);
    expectConfirmComponentState(expectedStateInvalid, ComponentClientReturnType::K_GENERAL_ERROR);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldTransitToKOffStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);
    {
        InSequence seq;
        expectGetComponentState(expectedStateKOff, ComponentClientReturnType::K_SUCCESS);
        expectConfirmComponentState(expectedStateKOff, ComponentClientReturnType::K_SUCCESS);
    }
    performActionOnState(stateSuspendMockPtr);
}

TEST_F(AppPollingModeTest, shouldTransitToKOnStateFromKOffWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);

    expectGetComponentState(expectedStateKOff, ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOff, ComponentClientReturnType::K_SUCCESS);
    performActionOnState(stateSuspendMockPtr);

    resume();
    performActionOnState(stateRunMockPtr);
}
