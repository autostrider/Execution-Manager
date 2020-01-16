#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/mean_calculator_mock.hpp"

#include <adaptive_app.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

class AppEventModeTest : public ::testing::Test
{
protected:
    std::unique_ptr<api::AppStateClientMock> stateClientMock{std::make_unique<StrictMock<api::AppStateClientMock>>()};

    std::unique_ptr<api::StateFactoryMock> factoryMock{std::make_unique<StrictMock<api::StateFactoryMock>>()};
    std::unique_ptr<api::StateMock> stateInitMock = std::make_unique<StrictMock<api::StateMock>>();

    std::unique_ptr<api::ComponentClientMock> compStateMock = std::make_unique<StrictMock<api::ComponentClientMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    api::StateMock* stateInitMockPtr = stateInitMock.get();
    api::StateFactoryMock* factoryPtr = factoryMock.get();
    api::ComponentClientMock* compStateMockPtr = compStateMock.get();

    void expectSetStateUpdateHandler();
    void expectCheckIfAnyEventsAvailable();

    void goToInit(AdaptiveApp&);
    void performActionOnState(AdaptiveApp&, api::StateMock *stateMock);
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

void AppEventModeTest::performActionOnState(AdaptiveApp& app, api::StateMock *stateMock)
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