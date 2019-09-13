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

class IAdaptiveAppMock : public api::IAdaptiveApp
{
public:
    MOCK_METHOD(void, run,());
    MOCK_METHOD(void, terminate,());

    MOCK_METHOD(void, reportApplicationState,
                (api::ApplicationStateClient::ApplicationState state));
};

class StateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        stateClientMock = std::make_unique<StateClientMock>();
        factoryMock = std::make_unique<IStateFactoryMock>();
    }
   std::unique_ptr<StateClientMock> stateClientMock{nullptr};
   std::unique_ptr<IStateFactoryMock> factoryMock{nullptr};
};

class FactoryTest : public ::testing::Test
{

protected:
    IStateFactoryMock factoryMock;
};

