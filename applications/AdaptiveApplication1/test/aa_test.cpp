#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <adaptive_app.hpp>

#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/mean_calculator_mock.hpp"


using namespace testing;

class AppTest : public ::testing::Test
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
    void goToTerminateFromState(api::StateMock *oldStateMock);
    void expectGoToSuspendFromState(api::StateMock *oldStateMock);
    void performActionOnState(api::StateMock *stateMock);
    void resume();
};
void AppTest::expectGetComponentState(const api::ComponentState& expectedState,
                                      const api::ComponentClientReturnType& result)
{
    EXPECT_CALL(*compStateMockPtr, GetComponentState(_))
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(expectedState),
                    Return(result))
                );
}

void AppTest::expectConfirmComponentState(const api::ComponentState& expectedState,
                                          const api::ComponentClientReturnType result)
{
    EXPECT_CALL(*compStateMockPtr,
                ConfirmComponentState
                (
                    Eq(expectedState),
                    result)
                );
}

void AppTest::goToInit()
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    app.init();
}

void AppTest::goToRunFromState(api::StateMock* oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*stateRunMock, enter());
    EXPECT_CALL(*factoryPtr,
                createRun(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));
    app.run();
}

void AppTest::goToTerminateFromState(api::StateMock *oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*stateTermMock, enter());
    EXPECT_CALL(*factoryPtr,
                createShutDown(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));
    app.terminate();
}


void AppTest::expectGoToSuspendFromState(api::StateMock *oldStateMock)
{
    EXPECT_CALL(*oldStateMock, leave());
    EXPECT_CALL(*factoryPtr, createSuspend(Ref(app))).WillOnce(Return(ByMove(std::move(stateSuspendMock))));
    EXPECT_CALL(*stateSuspendMockPtr, enter());
}

void AppTest::performActionOnState(api::StateMock *stateMock)
{
    EXPECT_CALL(*stateMock, performAction());
    app.performAction();
}

void AppTest::resume()
{
    {
        InSequence seq;

        stateRunMock = std::make_unique<StrictMock<api::StateMock>>();
        stateRunMockPtr = stateRunMock.get();

        expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
        EXPECT_CALL(*stateSuspendMockPtr, leave());
        EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
        EXPECT_CALL(*stateRunMockPtr, enter());
        expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);

    }
}

TEST_F(AppTest, Should_TransitToInitState)
{
    goToInit();
}

TEST_F(AppTest, Should_StayInKOnWhenPerformActionCalled)
{
    goToInit();
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kUnchanged);
    performActionOnState(stateInitMockPtr);
}

TEST_F(AppTest, Should_TransitToRunState)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
}

TEST_F(AppTest, Should_TransitToTerminateState)
{
    goToInit();
    goToTerminateFromState(stateInitMockPtr);
}

TEST_F(AppTest, shouldReturnMean)
{
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    app.mean();
}

TEST_F(AppTest, shouldReportErrorWhenErrorOccuredWhileGettingCompState)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kGeneralError);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kGeneralError);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppTest, shouldStayInKOnStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::kUnchanged);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppTest, shouldStayInKOnStateWhenInvalidStateReceived)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kInvalid);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppTest, shouldStayInKOnStateWhenGetComponentStateFailed)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);

    expectGetComponentState(expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    expectConfirmComponentState(expectedStateInvalid, api::ComponentClientReturnType::kGeneralError);
    performActionOnState(stateRunMockPtr);
}

TEST_F(AppTest, shouldTransitToKOffStateWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);
    {
        InSequence seq;
        expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
        expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    }
    performActionOnState(stateSuspendMockPtr);
}

TEST_F(AppTest, shouldTransitToKOnStateFromKOffWhenPerformActionCalled)
{
    goToInit();
    goToRunFromState(stateInitMockPtr);
    expectGoToSuspendFromState(stateRunMockPtr);

    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::kSuccess);
    performActionOnState(stateSuspendMockPtr);

    resume();
    performActionOnState(stateRunMockPtr);
}
