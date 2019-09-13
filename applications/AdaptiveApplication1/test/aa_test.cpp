#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

using namespace testing;


class IStateFactoryMock : public api::IStateFactory
{
public:
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit,(api::IAdaptiveApp &app));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun,(api::IAdaptiveApp &app));
    MOCK_METHOD(std::unique_ptr<api::IState>, createTerminate,(api::IAdaptiveApp &app));
};

class StateClientMock : public api::ApplicationStateClientWrapper
{
public:
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

class IStateMock : public api::IState
{
public:
    MOCK_METHOD(void, enter,());
    MOCK_METHOD(void, leave,(), (const));
};

class AppTest : public ::testing::Test
{
protected:

   std::unique_ptr<StateClientMock> stateClientMock{std::make_unique<StateClientMock>()};
   std::unique_ptr<IStateFactoryMock> factoryMock{std::make_unique<IStateFactoryMock>()};
};

TEST_F(AppTest, Should_TransitToInitState)
{

    std::unique_ptr<IStateMock> stateMock = std::make_unique<IStateMock>();
    EXPECT_CALL(*stateMock, enter()).WillOnce(Return());
    EXPECT_CALL(*stateMock, leave()).WillOnce(Return());

    EXPECT_CALL(*factoryMock,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateMock))));

     AdaptiveApp app{std::move(factoryMock),
                     nullptr};
     app.init();

}

TEST_F(AppTest, Should_TransitToRunState)
{
    std::unique_ptr<IStateMock> stateInitMock = std::make_unique<IStateMock>();
    EXPECT_CALL(*stateInitMock, enter()).WillOnce(Return());
    EXPECT_CALL(*stateInitMock, leave()).WillOnce(Return());

    EXPECT_CALL(*factoryMock,
                createInit((_)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    std::unique_ptr<IStateMock> stateRunMock = std::make_unique<IStateMock>();
    EXPECT_CALL(*stateRunMock, enter()).WillOnce(Return());
    EXPECT_CALL(*stateRunMock, leave()).WillOnce(Return());

    EXPECT_CALL(*factoryMock,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

     AdaptiveApp app{std::move(factoryMock),
                     nullptr};
     app.init();
     app.run();
}

TEST_F(AppTest, Should_TransitToTerminateState)
{
    std::unique_ptr<IStateMock> stateTermMock = std::make_unique<IStateMock>();
    EXPECT_CALL(*stateTermMock, enter()).WillOnce(Return());
    EXPECT_CALL(*stateTermMock, leave()).WillOnce(Return());

    EXPECT_CALL(*factoryMock,
                createTerminate((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

     AdaptiveApp app{std::move(factoryMock),
                     nullptr};
     app.terminate();
}

TEST_F(AppTest, ShouldCalculateMean)
{
    const double mu = 10;
    const double sigma = 0.5;

    AdaptiveApp app{std::move(factoryMock),
                    nullptr};

    app.readSensorData();
    bool result = ::abs(app.mean() - mu) < sigma;
    ASSERT_TRUE(result);
}

TEST_F(AppTest, Should_ReadSensorData)
{
    const double mu = 10;
    const double sigma = 0.5;

    AdaptiveApp app{std::move(factoryMock),
                    nullptr};

    app.printSensorData();
    ASSERT_EQ(app.mean(), 0.0);

    app.readSensorData();
    app.printSensorData();

    bool result = ::abs(app.mean() - mu) < sigma;
    ASSERT_TRUE(result);
}

TEST_F(AppTest, Should_ReturnZero_NoDataAvailable)
{
    AdaptiveApp app{std::move(factoryMock),
                    nullptr};

    app.printSensorData();
    ASSERT_EQ(app.mean(), 0.0);
}
