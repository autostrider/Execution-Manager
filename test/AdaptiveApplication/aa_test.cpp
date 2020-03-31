// #include <adaptive_app.hpp>

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

// class AppTest : public ::testing::Test
// {
// protected:
//     std::unique_ptr<AppStateClientMock> stateClientMock{std::make_unique<StrictMock<AppStateClientMock>>()};

//     std::unique_ptr<StateFactoryMock> factoryMock{std::make_unique<StrictMock<StateFactoryMock>>()};
//     std::unique_ptr<StateMock> stateInitMock = std::make_unique<StrictMock<StateMock>>();
//     std::unique_ptr<StateMock> stateRunMock = std::make_unique<StrictMock<StateMock>>();
//     std::unique_ptr<StateMock> stateTermMock = std::make_unique<StrictMock<StateMock>>();
//     std::unique_ptr<StateMock> stateSuspendMock = std::make_unique<StrictMock<StateMock>>();

//     std::unique_ptr<ComponentClientMock> compStateMock = std::make_unique<StrictMock<ComponentClientMock>>();
//     std::unique_ptr<MeanCalculatorMock> meanCalculatorMock = std::make_unique<StrictMock<MeanCalculatorMock>>();

//     StateMock* stateInitMockPtr = stateInitMock.get();
//     StateMock* stateRunMockPtr = stateRunMock.get();
//     StateMock* stateSuspendMockPtr = stateSuspendMock.get();
//     StateFactoryMock* factoryPtr = factoryMock.get();
//     AppStateClientMock* stateClientMockPtr = stateClientMock.get();
//     MeanCalculatorMock* meanCalculatorMockPtr = meanCalculatorMock.get();
//     ComponentClientMock* compStateMockPtr = compStateMock.get();

//     AdaptiveApp app{std::move(factoryMock), std::move(stateClientMock),
//                 std::move(compStateMock), std::move(meanCalculatorMock)};

//     void goToInit();
//     void goToRunFromState(StateMock *oldStateMock);
//     void goToTerminateFromState(StateMock *oldStateMock);
// };

// void AppTest::goToInit()
// {
//     EXPECT_CALL(*stateInitMock, enter());
//     EXPECT_CALL(*factoryPtr,
//                 createInit(Ref(app)))
//             .WillOnce(Return(ByMove(std::move(stateInitMock))));
//     app.init();
// }

// void AppTest::goToRunFromState(StateMock* oldStateMock)
// {
//     EXPECT_CALL(*oldStateMock, leave());
//     EXPECT_CALL(*stateRunMock, enter());
//     EXPECT_CALL(*factoryPtr,
//                 createRun(Ref(app)))
//             .WillOnce(Return(ByMove(std::move(stateRunMock))));
//     app.run();
// }

// void AppTest::goToTerminateFromState(StateMock *oldStateMock)
// {
//     EXPECT_CALL(*oldStateMock, leave());
//     EXPECT_CALL(*stateTermMock, enter());
//     EXPECT_CALL(*factoryPtr,
//                 createShutDown(Ref(app)))
//             .WillOnce(Return(ByMove(std::move(stateTermMock))));
//     app.terminate();
// }

// TEST_F(AppTest, Should_TransitToInitState)
// {
//     goToInit();
// }

// TEST_F(AppTest, Should_TransitToRunState)
// {
//     goToInit();
//     goToRunFromState(stateInitMockPtr);
// }

// TEST_F(AppTest, Should_TransitToTerminateState)
// {
//     goToInit();
//     goToTerminateFromState(stateInitMockPtr);
// }

// TEST_F(AppTest, shouldReturnMean)
// {
//     EXPECT_CALL(*meanCalculatorMockPtr, mean());
//     app.mean();
// }
