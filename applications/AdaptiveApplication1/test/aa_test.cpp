// #include "gtest/gtest.h"
// #include "gmock/gmock.h"
// #include <iostream>
// #include <atomic>

// #include <adaptive_app.hpp>
// #include <state.hpp>

// using namespace testing;

// class IStateFactoryMock : public api::IStateFactory
// {
// public:
//     MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& app), (const));
//     MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& app), (const));
//     MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& app), (const));
// };

// class StateClientMock : public api::ApplicationStateClientWrapper
// {
// public:
//     MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
// };

// class IStateMock : public api::IState
// {
// public:
//     MOCK_METHOD(void, enter, ());
//     MOCK_METHOD(void, leave, (), (const));
// };

// class AppTest : public ::testing::Test
// {
// protected:
//    std::unique_ptr<StateClientMock> stateClientMock{std::make_unique<StateClientMock>()};
//    std::unique_ptr<IStateFactoryMock> factoryMock{std::make_unique<IStateFactoryMock>()};
//    std::unique_ptr<NiceMock<IStateMock>> stateInitMock = std::make_unique<NiceMock<IStateMock>>();
//    std::unique_ptr<NiceMock<IStateMock>> stateRunMock = std::make_unique<NiceMock<IStateMock>>();
//    std::unique_ptr<NiceMock<IStateMock>> stateTermMock = std::make_unique<NiceMock<IStateMock>>();
// };

// TEST_F(AppTest, Should_TransitToInitState)
// {
//     EXPECT_CALL(*factoryMock,
//                 createInit((_)))
//             .WillOnce(Return(ByMove(std::move(stateInitMock))));

//      AdaptiveApp app{std::move(factoryMock),
//                      nullptr, nullptr};
//      app.init();

// }

// TEST_F(AppTest, Should_TransitToRunState)
// {
//     EXPECT_CALL(*factoryMock,
//                 createInit((_)))
//             .WillOnce(Return(ByMove(std::move(stateInitMock))));

//     EXPECT_CALL(*factoryMock,
//                 createRun((_)))
//             .WillOnce(Return(ByMove(std::move(stateRunMock))));

//      AdaptiveApp app{std::move(factoryMock),
//                      nullptr, nullptr};
//      app.init();
//      app.run();
// }

// TEST_F(AppTest, Should_TransitToTerminateState)
// {
//     EXPECT_CALL(*factoryMock,
//                 createInit((_)))
//             .WillOnce(Return(ByMove(std::move(stateInitMock))));
//     EXPECT_CALL(*factoryMock,
//                 createShutDown((_)))
//             .WillOnce(Return(ByMove(std::move(stateTermMock))));

//      AdaptiveApp app{std::move(factoryMock),
//                      nullptr, nullptr};
//      app.init();
//      app.terminate();
// }

// TEST_F(AppTest, WhenSensorDataRead)
// {
//     const double mu = 10;
//     const double sigma = 0.5;

//     AdaptiveApp app{std::move(factoryMock),
//                     nullptr, nullptr};

//     app.readSensorData();
//     bool result = ::abs(app.mean() - mu) < sigma;
//     ASSERT_TRUE(result);
// }

// TEST_F(AppTest, Should_ReadSensorData)
// {
//     const double mu = 10;
//     const double sigma = 0.5;

//     AdaptiveApp app{std::move(factoryMock),
//                     nullptr, nullptr};

//     ASSERT_EQ(app.mean(), 0.0);

//     app.readSensorData();

//     bool result = ::abs(app.mean() - mu) < sigma;
//     ASSERT_TRUE(result);
// }

// TEST_F(AppTest, WhenSensorNotDataRead)
// {
//     AdaptiveApp app{std::move(factoryMock),
//                     nullptr, nullptr};

//     ASSERT_EQ(app.mean(), 0.0);
// }

// TEST_F(AppTest, Should_ReportCurrentState)
// {
//     EXPECT_CALL(*stateClientMock, ReportApplicationState(_)).WillOnce(Return());

//     AdaptiveApp app{nullptr, std::move(stateClientMock), nullptr};
//     app.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
// }
