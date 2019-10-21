#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

using namespace testing;

class StateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& app), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& app), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& app), (const));
};

class ApplicationStateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

class StateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

class AppTest : public ::testing::Test
{
protected:
    std::unique_ptr<ApplicationStateClientMock> stateClientMock{std::make_unique<StrictMock<ApplicationStateClientMock>>()};
    std::unique_ptr<StateFactoryMock> factoryMock{std::make_unique<StateFactoryMock>()};
    std::unique_ptr<StateMock> stateInitMock = std::make_unique<NiceMock<StateMock>>();
    std::unique_ptr<StateMock> stateRunMock = std::make_unique<NiceMock<StateMock>>();
    std::unique_ptr<StateMock> stateTermMock = std::make_unique<NiceMock<StateMock>>();
};

TEST_F(AppTest, Should_TransitToInitState)
{
    auto factoryPtr = factoryMock.get();
    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();

}

TEST_F(AppTest, Should_TransitToRunState)
{
    auto factoryPtr = factoryMock.get();
    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    EXPECT_CALL(*factoryPtr,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    EXPECT_CALL(*factoryPtr,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

    app.init();
    app.run();
}

TEST_F(AppTest, Should_TransitToTerminateState)
{
    auto factoryPtr = factoryMock.get();
    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    EXPECT_CALL(*factoryPtr,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));
    EXPECT_CALL(*factoryPtr,
                createShutDown((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

    app.init();
    app.terminate();
}

TEST_F(AppTest, WhenSensorDataRead)
{
    const double mu = 10;
    const double sigma = 0.5;

    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    app.readSensorData();
    bool result = ::abs(app.mean() - mu) < sigma;
    ASSERT_TRUE(result);
}

TEST_F(AppTest, Should_ReadSensorData)
{
    const double mu = 10;
    const double sigma = 0.5;

    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    ASSERT_EQ(app.mean(), 0.0);

    app.readSensorData();

    bool result = ::abs(app.mean() - mu) < sigma;
    ASSERT_TRUE(result);
}

TEST_F(AppTest, WhenSensorNotDataRead)
{
    AdaptiveApp app{std::move(factoryMock),
                nullptr, nullptr};

    ASSERT_EQ(app.mean(), 0.0);
}

TEST_F(AppTest, Should_ReportCurrentState)
{
    EXPECT_CALL(*stateClientMock, ReportApplicationState(_)).WillOnce(Return());

    AdaptiveApp app{nullptr, std::move(stateClientMock), nullptr};
    app.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
}
