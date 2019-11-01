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

    const api::ComponentState expectedStateKOn = api::ComponentStateKOn;
    const api::ComponentState expectedStateKOff = api::ComponentStateKOff;
    const api::ComponentState expectedStateInvalid = "invalidState";

    void expectGetComponentState(const api::ComponentState &expectedState,
                                 const api::ComponentClientReturnType& result);
    void expectConfirmComponentState(const api::ComponentState& expectedState,
                                     api::ComponentClientReturnType result);
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

TEST_F(AppTest, Should_TransitToInitState)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();
}

TEST_F(AppTest, Should_TransitToRunState)
{

    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*stateInitMock, leave());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();

    EXPECT_CALL(*stateRunMock, enter());
    EXPECT_CALL(*factoryPtr,
                createRun(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

    app.run();
}

TEST_F(AppTest, Should_TransitToTerminateState)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*stateInitMock, leave());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();

    EXPECT_CALL(*stateTermMock, enter());
    EXPECT_CALL(*factoryPtr,
                createShutDown(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

    app.terminate();
}

TEST_F(AppTest, shouldReturnMean)
{
    EXPECT_CALL(*meanCalculatorMockPtr, mean());
    app.mean();
}

TEST_F(AppTest, shouldReportErrorWhenErrorOccuredWhileGettingCompState)
{
    EXPECT_CALL(*factoryPtr, createInit(Ref(app))).WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*stateInitMockPtr, enter());
    EXPECT_CALL(*stateInitMockPtr, leave());
    app.init();

    EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
    EXPECT_CALL(*stateRunMockPtr, enter());
    EXPECT_CALL(*stateRunMockPtr, performAction());
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_GENERAL_ERROR);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_GENERAL_ERROR);

    app.run();
    app.performAction();
}

TEST_F(AppTest, shouldStayInKonStateWhenPerformActionCalled)
{
    EXPECT_CALL(*factoryPtr, createInit(Ref(app))).WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*stateInitMockPtr, enter());
    EXPECT_CALL(*stateInitMockPtr, leave());
    app.init();

    EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
    EXPECT_CALL(*stateRunMockPtr, enter());
    EXPECT_CALL(*stateRunMockPtr, performAction());
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_UNCHANGED);

    app.run();
    app.performAction();
}

TEST_F(AppTest, shouldTransitToKoffStateWhenPerformActionCalled)
{
    EXPECT_CALL(*factoryPtr, createInit(Ref(app))).WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*stateInitMockPtr, enter());
    EXPECT_CALL(*stateInitMockPtr, leave());
    app.init();

    EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
    EXPECT_CALL(*factoryPtr, createSuspend(Ref(app))).WillOnce(Return(ByMove(std::move(stateSuspendMock))));
    EXPECT_CALL(*stateRunMockPtr, enter());
    EXPECT_CALL(*stateRunMockPtr, leave());
    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);

    app.run();

    EXPECT_CALL(*stateSuspendMockPtr, enter());
    EXPECT_CALL(*stateSuspendMockPtr, performAction());
    app.performAction();
}


TEST_F(AppTest, shouldTransitToKOnStateFromKOffWhenPerformActionCalled)
{
    EXPECT_CALL(*factoryPtr, createInit(Ref(app))).WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*stateInitMockPtr, enter());
    EXPECT_CALL(*stateInitMockPtr, leave());
    app.init();

    EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
    EXPECT_CALL(*factoryPtr, createSuspend(Ref(app))).WillOnce(Return(ByMove(std::move(stateSuspendMock))));
    EXPECT_CALL(*stateRunMockPtr, enter());
    EXPECT_CALL(*stateRunMockPtr, leave());
    expectGetComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOff, api::ComponentClientReturnType::K_SUCCESS);

    app.run();

    EXPECT_CALL(*stateSuspendMockPtr, enter());
    EXPECT_CALL(*stateSuspendMockPtr, performAction());
    EXPECT_CALL(*stateSuspendMockPtr, leave());
    app.performAction();

    stateRunMock = std::make_unique<StrictMock<api::StateMock>>();
    stateRunMockPtr = stateRunMock.get();
    EXPECT_CALL(*factoryPtr, createRun(Ref(app))).WillOnce(Return(ByMove(std::move(stateRunMock))));
    EXPECT_CALL(*stateRunMockPtr, enter());
    EXPECT_CALL(*stateRunMockPtr, performAction());
    expectGetComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);
    expectConfirmComponentState(expectedStateKOn, api::ComponentClientReturnType::K_SUCCESS);

    app.performAction();
}
