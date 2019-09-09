#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

using testing::Return;
using testing::ByMove;
using testing::_;

class InitMock : public Init
{
public:
    InitMock(AdaptiveApp& app) : Init(app){}
    MOCK_METHOD0(handleTransition, std::unique_ptr<State>());
    MOCK_METHOD0(enter, void());
    MOCK_CONST_METHOD0(leave, void());
};

class ClientMock : public api::ApplicationStateClient
{
public:
    MOCK_METHOD1(ReportApplicationState, void (ApplicationState state));
};

TEST(DeleteApp, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp* app = new AdaptiveApp(term);
    delete app;
}
TEST(ReadSensorDataSucces, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);
    app.readSensorData();
    ASSERT_NE(app.mean(), 0.0);
}

TEST(TerminateApp, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);
    EXPECT_EQ(app.isTerminating(), term);
    term = true;
    EXPECT_EQ(app.isTerminating(), term);
}

TEST(MeanSucces, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);
    app.printSensorData();
    ASSERT_EQ(app.mean(), 0);
}

TEST(InitSucces, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);

    auto mock = std::make_unique<InitMock>(app);
    EXPECT_CALL(*mock, enter());
    app.init(std::move(mock), nullptr);
}

TEST(TransitToRunStateSucces, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);

    auto stateMock = std::make_unique<InitMock>(app);
    auto clientMock = std::make_unique<ClientMock>();

    EXPECT_CALL(*stateMock, enter());
    EXPECT_CALL(*stateMock, leave());
    EXPECT_CALL(*stateMock, handleTransition)
            .WillOnce(Return(ByMove(std::make_unique<::Run>(app))));
    EXPECT_CALL(*clientMock, ReportApplicationState(_)).WillRepeatedly(Return());

    app.init(std::move(stateMock), std::move(clientMock));

    app.transitToNextState();
}

TEST(ReportCurrentStateSucces, AppTest)
{
    std::atomic<bool> term{false};
    AdaptiveApp app(term);

    auto stateMock = std::make_unique<InitMock>(app);
    auto clientMock = std::make_unique<ClientMock>();

    EXPECT_CALL(*stateMock, enter());
    EXPECT_CALL(*clientMock, ReportApplicationState(_)).WillRepeatedly(Return());

    app.init(std::move(stateMock), std::move(clientMock));

    app.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_RUNNING);
}
