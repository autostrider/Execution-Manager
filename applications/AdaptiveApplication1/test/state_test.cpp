#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>


using namespace testing;

using ApplicationState = api::ApplicationStateClient::ApplicationState;

class IStateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit,(api::IAdaptiveApp &app));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun,(api::IAdaptiveApp &app));
    MOCK_METHOD(std::unique_ptr<api::IState>, createTerminate,(api::IAdaptiveApp &app));
};

class IStateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter,());
    MOCK_METHOD(void, leave,(), (const));
};

class StateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD1(ReportApplicationState, void(ApplicationStateManagement::ApplicationState state));
};

class IAdaptiveAppMock : public AdaptiveApp
{
public:
    IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
    std::unique_ptr<api::IApplicationStateClientWrapper> client);

    MOCK_METHOD(void, init, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, terminate, ());

    MOCK_METHOD(void, reportApplicationState,
                (api::ApplicationStateClient::ApplicationState state));
    MOCK_METHOD(void, transitToNextState, (IAdaptiveApp::FactoryFunc nextState));

    MOCK_METHOD(double, mean, ());
    MOCK_METHOD(void, readSensorData, ());
};

IAdaptiveAppMock::IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
                                   std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    AdaptiveApp (std::move(factory), std::move(client))
{

}

class StateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        stateClientMock = std::make_unique<StateClientMock>();
        factoryMock = std::make_unique<IStateFactoryMock>();
        appMock = new IAdaptiveAppMock{std::move(factoryMock), std::move(stateClientMock)};
    }

    void TearDown() override
    {
        delete appMock;
    }

   std::unique_ptr<StateClientMock> stateClientMock{nullptr};
   std::unique_ptr<IStateFactoryMock> factoryMock{nullptr};
   IAdaptiveAppMock* appMock{nullptr};
};

TEST_F(StateTest, Should_InitCallEnter)
{
    EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*appMock);
    state->enter();
}

TEST_F(StateTest, Should_RunCallEnter)
{
    //EXPECT_CALL(*appMock, mean()).WillOnce(Return(0));
    //EXPECT_CALL(*appMock, readSensorData()).WillOnce(Return());

    std::unique_ptr<::Run> state = std::make_unique<::Run>(*appMock);
    state->enter();
}

TEST_F(StateTest, Should_InitCallLeave)
{
    EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*appMock);
    state->leave();
}

TEST_F(StateTest, Should_TerminateCallEnter)
{
    EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Terminate> state = std::make_unique<Terminate>(*appMock);
    state->enter();
}
