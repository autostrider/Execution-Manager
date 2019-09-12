#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

using namespace testing;

/*
class IStateFactoryMock : public IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<State>,
                buildState,
                (api::ApplicationStateClient::ApplicationState state,
                                                             AdaptiveApp& app));
};

class StateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD1(ReportApplicationState, void(ApplicationStateManagement::ApplicationState state));
};

class AppTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        //factoryMock = std::make_unique<IStateFactoryMock>();
        //stateClientMock = std::make_unique<StateClientMock>();
        //app = AdaptiveApp(std::move(factoryMock),
         //                  std::move(stateClientMock));
    }

   std::unique_ptr<StateClientMock> stateClientMock{std::make_unique<StateClientMock>()};
   std::unique_ptr<IStateFactoryMock> factoryMock{std::make_unique<IStateFactoryMock>()};
  // AdaptiveApp app{std::move(factoryMock),
   //                std::move(stateClientMock)};
};
/*
TEST_F(AppTest, AppInInitState)
{

    EXPECT_CALL(*factoryMock,
                buildState(api::ApplicationStateClient::ApplicationState::K_INITIALIZING, (_)))
            .WillOnce(Return(ByMove(std::unique_ptr<Init>())));
     AdaptiveApp app{std::move(factoryMock),
                     std::move(stateClientMock)};

}

*/

