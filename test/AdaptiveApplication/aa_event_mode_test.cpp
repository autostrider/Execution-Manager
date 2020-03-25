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

class AppEventModeTest : public ::testing::Test
{
protected:
    std::unique_ptr<AppStateClientMock> stateClientMock{std::make_unique<StrictMock<AppStateClientMock>>()};

    std::unique_ptr<StateFactoryMock> factoryMock{std::make_unique<StrictMock<StateFactoryMock>>()};
    std::unique_ptr<StateMock> stateInitMock = std::make_unique<StrictMock<StateMock>>();

    std::unique_ptr<ComponentClientMock> compStateMock = std::make_unique<StrictMock<ComponentClientMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    StateMock* stateInitMockPtr = stateInitMock.get();
    StateFactoryMock* factoryPtr = factoryMock.get();
    ComponentClientMock* compStateMockPtr = compStateMock.get();

    void expectSetStateUpdateHandler();
    void expectCheckIfAnyEventsAvailable();

    void goToInit(AdaptiveApp&);
    void performActionOnState(AdaptiveApp&, StateMock *stateMock);
    std::unique_ptr<AdaptiveApp> createApp();
};

std::unique_ptr<AdaptiveApp> AppEventModeTest::createApp()
{
    expectSetStateUpdateHandler();

    return std::make_unique<AdaptiveApp>(std::move(factoryMock), std::move(stateClientMock),
                                         std::move(compStateMock), std::move(meanCalculatorMock), true);
}

void AppEventModeTest::expectSetStateUpdateHandler()
{    
    EXPECT_CALL(*compStateMockPtr, SetStateUpdateHandler(_));
}

void AppEventModeTest::expectCheckIfAnyEventsAvailable()
{    
    EXPECT_CALL(*compStateMockPtr, CheckIfAnyEventsAvailable());
}

void AppEventModeTest::goToInit(AdaptiveApp& app)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    app.init();
}

void AppEventModeTest::performActionOnState(AdaptiveApp& app, StateMock *stateMock)
{
    EXPECT_CALL(*stateMock, performAction());
    expectCheckIfAnyEventsAvailable();

    app.performAction();
}

TEST_F(AppEventModeTest, shouldCreateAppWithEventModeAndStartPerformAction)
{
    auto application = createApp();
    goToInit(*application);
    performActionOnState(*application, stateInitMockPtr);
}