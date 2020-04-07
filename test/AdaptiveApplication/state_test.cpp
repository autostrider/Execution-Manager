// #include <state.hpp>

// #include <i_state_mock.hpp>
// #include <i_state_factory_mock.hpp>
// #include <app_state_client_mock.hpp>
// #include <component_client_mock.hpp>
// #include <mean_calculator_mock.hpp>

// #include "gtest/gtest.h"
// #include "gmock/gmock.h"

// using namespace api;
// using namespace application_state;
// using namespace component_client;
// using namespace testing;

// using ApplicationState = ApplicationStateClient::ApplicationState;

// class StateTest : public ::testing::Test
// {
// protected:
//     std::unique_ptr<AppStateClientMock> stateClientMock = std::make_unique<StrictMock<AppStateClientMock>>();
//     std::unique_ptr<ComponentClientMock> componentClientMock = std::make_unique<StrictMock<ComponentClientMock>>();
//     std::unique_ptr<StateFactoryMock> factoryMock = std::make_unique<StrictMock<StateFactoryMock>>();
//     std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();
//     StateFactory factory;

//     AppStateClientMock* stateClientMockPtr = stateClientMock.get();
//     ComponentClientMock* componentClientMockPtr = componentClientMock.get();
//     MeanCalculatorMock* meanCalculatorMockPtr = meanCalculatorMock.get();

//     AdaptiveApp appMock{std::move(factoryMock),
//                         std::move(stateClientMock),
//                         std::move(componentClientMock),
//                         std::move(nullptr),
//                         std::move(meanCalculatorMock)};
// };

// TEST_F(StateTest, shouldReportStateWhenInitEntered)
// {
//     auto state = factory.createInit(appMock);
//     EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationState::kInitializing));
//     state->enter();
// }

// TEST_F(StateTest, shouldReportRunningStateWhenRunEntered)
// {
//     auto state = factory.createRun(appMock);
//     EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationStateClient::ApplicationState::kRunning));
//     state->enter();
// }

// TEST_F(StateTest, shouldCalculateMeanWhenPerforemActionCalled)
// {
//     auto state = factory.createRun(appMock);
//     EXPECT_CALL(*meanCalculatorMockPtr, mean());
//     state->performAction();
// }

// TEST_F(StateTest, shouldReportShutdownStateWhenShutDownEntered)
// {
//     auto state = factory.createShutDown(appMock);
//     EXPECT_CALL(*stateClientMockPtr, ReportApplicationState(ApplicationStateClient::ApplicationState::kShuttingdown));
//     state->enter();
// }

// TEST_F(StateTest, shouldDoNothingWhenShutdownLeft)
// {
//     auto state = factory.createShutDown(appMock);
//     state->leave();
// }

// TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForInitState)
// {
//     auto state = factory.createInit(appMock);
//     state->performAction();
// }
// TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForSuspendState)
// {
//     auto state = factory.createSuspend(appMock);
//     state->performAction();
// }

// TEST_F(StateTest, shouldDoNothingWhenPerformActionCalledForShutdownState)
// {
//     auto state = factory.createShutDown(appMock);
//     state->performAction();
// }

// TEST_F(StateTest, shouldDoNothingWhenSuspendEntered)
// {
//     auto state = factory.createSuspend(appMock);
//     state->leave();
// }
// TEST_F(StateTest, shouldDoNothingWhenSuspendLeft)
// {
//     auto state = factory.createSuspend(appMock);
//     state->enter();
// }
