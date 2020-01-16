#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <adaptive_app.hpp>

#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/mean_calculator_mock.hpp"


using namespace testing;

class AppPollingModeTest : public ::testing::Test
{
protected:
    std::unique_ptr<api::AppStateClientMock> stateClientMock{std::make_unique<StrictMock<api::AppStateClientMock>>()};

    std::unique_ptr<api::StateFactoryMock> factoryMock{std::make_unique<StrictMock<api::StateFactoryMock>>()};
    std::unique_ptr<api::StateMock> stateInitMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::StateMock> stateRunMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::StateMock> stateTermMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::StateMock> stateSuspendMock = std::make_unique<StrictMock<api::StateMock>>();

    std::unique_ptr<api::ComponentClientMock> compStateMock = std::make_unique<StrictMock<api::ComponentClientMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    api::StateMock* stateInitMockPtr = stateInitMock.get();
    api::StateMock* stateRunMockPtr = stateRunMock.get();
    api::StateMock* stateSuspendMockPtr = stateSuspendMock.get();
    api::StateFactoryMock* factoryPtr = factoryMock.get();
    api::AppStateClientMock* stateClientMockPtr = stateClientMock.get();
    MeanCalculatorMock* meanCalculatorMockPtr = meanCalculatorMock.get();
    api::ComponentClientMock* compStateMockPtr = compStateMock.get();

    AdaptiveApp app{std::move(factoryMock), std::move(stateClientMock),
                std::move(compStateMock), std::move(meanCalculatorMock)};

    const api::ComponentState expectedStateKOn = COMPONENT_STATE_ON;
    const api::ComponentState expectedStateKOff = COMPONENT_STATE_OFF;
    const api::ComponentState expectedStateInvalid = "invalidState";

    void expectGetComponentState(const api::ComponentState &expectedState,
                                 const api::ComponentClientReturnType& result);
    void expectConfirmComponentState(const api::ComponentState& expectedState,
                                     api::ComponentClientReturnType result);
    void goToInit();
    void goToRunFromState(api::StateMock *oldStateMock);
    void expectGoToSuspendFromState(api::StateMock *oldStateMock);
    void performActionOnState(api::StateMock *stateMock);
    void resume();
};

void AppPollingModeTest::expectGetComponentState(const api::ComponentState& expectedState,
                                      const api::ComponentClientReturnType& result)
{
    EXPECT_CALL(*compStateMockPtr, GetComponentState(_))
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(expectedState),
                    Return(result))
                );
}

void AppPollingModeTest::expectConfirmComponentState(const api::ComponentState& expectedState,
                                          const api::ComponentClientReturnType result)
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

void AppPollingModeTest::goToRunFromState(api::StateMock* oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*stateRunMock, enter());
    EXPECT_CALL(*factoryPtr,
                createRun(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));
    app.run();
}

void AppPollingModeTest::expectGoToSuspendFromState(api::StateMock *oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*factoryPtr, createSuspend(Ref(app))).WillOnce(Return(ByMove(std::move(stateSuspendMock))));
    EXPECT_CALL(*stateSuspendMockPtr, enter());
}

void AppPollingModeTest::performActionOnState(api::StateMock *stateMock)
{
    EXPECT_CALL(*stateMock, performAction());
    app.performAction();
}

void AppPollingModeTest::resume()
{
    {
        InSequence seq;

        stateRunMock = std::make_unique<StrictMock<api::StateMock>>();
        stateRunMockPtr = stateRunMock.get();

        expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);
        EXPECT_CALL(*stateSuspendMockPtr, leave());
        EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
        EXPECT_CALL(*stateRunMockPtr, enter());
        expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);

    }
}

TEST_F(AppPollingModeTest, Should_StayInKOnWhenPerformActionCalled)
{
    goToInit();
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_UNCHANGED);
    performActionOnState(stateInitMockPtr);
}

TEST_F(AppPollingModeTest, shouldReportErrorWhenErrorOccuredWhileGettingCompState)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_GENERAL_ERROR);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_GENERAL_ERROR);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_UNCHANGED);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenInvalidStateReceived)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::K_INVALID);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldStayInKOnStateWhenGetComponentStateFailed)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::K_GENERAL_ERROR);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::K_GENERAL_ERROR);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppPollingModeTest, shouldTransitToKOffStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);
    {
        InSequence seq;
        expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
        expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
    }
    performActionOnState(stateSuspendMockPtr);
}

TEST_F(AppPollingModeTest, shouldTransitToKOnStateFromKOffWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
    performActionOnState(stateSuspendMockPtr);

    resume();
    performActionOnState(stateRunMockPtr);
}
