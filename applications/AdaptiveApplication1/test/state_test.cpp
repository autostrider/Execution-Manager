#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

using testing::Return;
using testing::ByMove;
using testing::_;
/*
using ApplicationState = api::ApplicationStateClient::ApplicationState;
class AppMock : public AdaptiveApp
{
public:
    AppMock(std::atomic<bool>& term) : AdaptiveApp (term){}
    virtual ~AppMock() = default;

    MOCK_CONST_METHOD0(isTerminating, bool());
    MOCK_METHOD1(reportApplicationState, void(ApplicationState state));
    MOCK_METHOD0(mean, double());
    MOCK_METHOD0(readSensorData, void());
};

TEST(DeleteState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    State* state = new ::Run(app);
    delete state;
}
TEST(fromInitToRunState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Init>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(false));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_INITIALIZING);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_RUNNING);
}
TEST(fromInitToTerminateState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Init>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(true));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_INITIALIZING);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);
}

TEST(enterInit, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Init>(app);

    EXPECT_CALL(app, reportApplicationState(_)).Times(2).WillRepeatedly(Return());
    state->enter();
    state->leave();
}

TEST(enterRun, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<::Run>(app);

    EXPECT_CALL(app, readSensorData()).WillOnce(Return());
    EXPECT_CALL(app, mean()).WillOnce(Return(0.0));
    state->enter();
    state->leave();
}

TEST(fromRunToTerminateState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<::Run>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(true));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_RUNNING);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);
}

TEST(fromRunToRunState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<::Run>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(false));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_RUNNING);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_RUNNING);
}

TEST(fromTerminateToTerminateStateSignal, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Terminate>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(true));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);
}

TEST(fromTerminateToTerminateState, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Terminate>(app);

    EXPECT_CALL(app, isTerminating()).WillOnce(Return(false));
    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);

    state = state->handleTransition();

    ASSERT_EQ(state->getApplicationState(), ApplicationState::K_SHUTTINGDOWN);
}

TEST(enterTerminate, StateTest)
{
    std::atomic<bool> term{false};
    AppMock app(term);
    std::unique_ptr<State> state = std::make_unique<Terminate>(app);

    EXPECT_CALL(app, reportApplicationState(_)).WillOnce(Return());
    EXPECT_THROW(state->enter(), std::runtime_error);
}
*/