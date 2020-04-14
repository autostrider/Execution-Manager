#include <adaptive_app.hpp>

#include <i_state_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <app_state_client_mock.hpp>
#include <component_client_mock.hpp>
#include <mean_calculator_mock.hpp>
#include <i_component_client_wrapper.hpp>
#include <client_mock.hpp>

#include <safe_queue.hpp>

#include <any.pb.h>
#include <execution_management.pb.h>

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
    std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

    std::unique_ptr<ClientMock> clientMock = std::make_unique<StrictMock<ClientMock>>();
    
    ClientMock* clientMockPtr = clientMock.get();
    StateMock* stateInitMockPtr = stateInitMock.get();
    StateFactoryMock* factoryPtr = factoryMock.get();
    ComponentClientMock* compClientMockPtr = compClientMock.get();

    void expectSetStateUpdateHandler();
    void expectCheckIfAnyEventsAvailable();
    void expectConfirm();

    void goToInit(AdaptiveApp&);
    void performActionOnState(AdaptiveApp&, StateMock *stateMock);

    std::unique_ptr<AdaptiveApp> createApp();
};

std::unique_ptr<AdaptiveApp> AppEventModeTest::createApp()
{
    return std::make_unique<AdaptiveApp>(std::move(factoryMock), std::move(stateClientMock),
                                         std::move(compClientMock),
                                         std::move(meanCalculatorMock), true);
}

void AppEventModeTest::expectSetStateUpdateHandler()
{
    EXPECT_CALL(*compClientMockPtr, setStateUpdateHandler(_));
}

void AppEventModeTest::expectCheckIfAnyEventsAvailable()
{    
    EXPECT_CALL(*compClientMockPtr, checkIfAnyEventsAvailable());
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

void AppEventModeTest::performActionOnState(AdaptiveApp& app, StateMock *stateMock)
{
    EXPECT_CALL(*stateMock, performAction());
    expectCheckIfAnyEventsAvailable();

    app.performAction();
}

TEST_F(AppEventModeTest, shouldCreateAppWithEventModeAndStartPerformAction)
{
    expectSetStateUpdateHandler();

    auto application = createApp();
    goToInit(*application);
    performActionOnState(*application, stateInitMockPtr);
}

TEST_F(AppEventModeTest, shouldSuccessfullySetComponentState)
{
    ExecutionManagement::setComponentState context;
    context.set_state("kOn");

    google::protobuf::Any message;
    message.PackFrom(context);

    size_t size = message.ByteSizeLong();
    std::vector<uint8_t> rawData(size);
        
    message.SerializeToArray(rawData.data(), size);

    std::string data(rawData.cbegin(), rawData.cend());



    ExecutionManagement::resultRegisterComponent cont;
    cont.set_result(ComponentClientReturnType::kSuccess);

    google::protobuf::Any mess;
    mess.PackFrom(cont);

    size_t size1 = mess.ByteSizeLong();
    std::vector<uint8_t> rawData1(size1);
        
    mess.SerializeToArray(rawData1.data(), size1);

    std::string data1(rawData1.cbegin(), rawData1.cend());

    {
        InSequence sq;
        EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
        EXPECT_CALL(*clientMockPtr, sendMessage(_)).WillOnce(Return());
        EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(
                    DoAll(
                        SetArgReferee<0>(data1),
                        Return(size1))
                    );
        EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(
                    DoAll(
                        SetArgReferee<0>(data),
                        Return(size))
                    );
        EXPECT_CALL(*clientMockPtr, sendMessage(_));
        EXPECT_CALL(*stateInitMockPtr, performAction());
    }

    std::unique_ptr<ComponentClientWrapper> componentClient =
        std::make_unique<ComponentClientWrapper>("ComponentWithEventMode", component_client::StateUpdateMode::kEvent);

    componentClient->setClient(std::move(clientMock));

    auto application = std::make_unique<AdaptiveApp>(std::move(factoryMock), std::move(stateClientMock),
                                         std::move(componentClient),
                                         std::move(meanCalculatorMock), true);

    goToInit(*application);

    application->performAction();
}