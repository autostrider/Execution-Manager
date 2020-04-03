#include <adaptive_app.hpp>

#include <i_state_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <app_state_client_mock.hpp>
#include <component_client_mock.hpp>
#include <component_server_mock.hpp>
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

    std::unique_ptr<ComponentClientMock> compClientMock = std::make_unique<StrictMock<ComponentClientMock>>();
    std::unique_ptr<ComponentServerMock> compServerMock = std::make_unique<StrictMock<ComponentServerMock>>();
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    StateMock* stateInitMockPtr = stateInitMock.get();
    StateFactoryMock* factoryPtr = factoryMock.get();
    ComponentClientMock* compClientMockPtr = compClientMock.get();
    ComponentServerMock* compServerMockPtr = compServerMock.get();

    void expectStartServer();
    void expectGetElements(bool);
    void expectSetStateUpdateHandler();
    void expectConfirm();

    void goToInit(AdaptiveApp&);
    void performActionOnState(AdaptiveApp&, StateMock *stateMock, bool);
    std::unique_ptr<AdaptiveApp> createApp();
};

std::unique_ptr<AdaptiveApp> AppEventModeTest::createApp()
{
    expectStartServer();

    return std::make_unique<AdaptiveApp>(std::move(factoryMock), std::move(stateClientMock),
                                         std::move(compClientMock), std::move(compServerMock),
                                         std::move(meanCalculatorMock), true);
}

void AppEventModeTest::expectStartServer()
{
    EXPECT_CALL(*compServerMockPtr, start());
}

void AppEventModeTest::expectGetElements(bool res)
{
    EXPECT_CALL(*compServerMockPtr, getQueueElement(_)).WillOnce(Return(res));
}

void AppEventModeTest::expectSetStateUpdateHandler()
{
    EXPECT_CALL(*compServerMockPtr, setStateUpdateHandler(_));
}

void AppEventModeTest::expectConfirm()
{
    EXPECT_CALL(*compClientMockPtr, ConfirmComponentState(_, _));
}

void AppEventModeTest::goToInit(AdaptiveApp& app)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    app.init();
}

void AppEventModeTest::performActionOnState(AdaptiveApp& app, StateMock *stateMock, bool resualt)
{
    EXPECT_CALL(*stateMock, performAction());
    expectGetElements(resualt);
    
    if(resualt)
    {
        expectSetStateUpdateHandler();
        expectConfirm();
    }

    app.performAction();
}

TEST_F(AppEventModeTest, shouldCreateAppWithEventModeAndStartPerformAction)
{
    bool resualt = false;
    auto application = createApp();
    goToInit(*application);
    performActionOnState(*application, stateInitMockPtr, resualt);
}

TEST_F(AppEventModeTest, shouldSuccessfullySetComponentState)
{
    bool resualt = true;
    auto application = createApp();
    goToInit(*application);
    performActionOnState(*application, stateInitMockPtr, resualt);
}