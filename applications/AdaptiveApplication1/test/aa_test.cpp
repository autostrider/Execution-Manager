#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <atomic>

#include <adaptive_app.hpp>
#include <state.hpp>

#include "mocks/i_state_mock.hpp"
#include "mocks/i_state_factory_mock.hpp"
#include "mocks/app_state_client_mock.hpp"
#include "mocks/component_client_mock.hpp"
#include "mocks/adaptive_app_base_mock.hpp"


using namespace testing;

class AppTest : public ::testing::Test
{
protected:
    std::unique_ptr<api::AppStateClientMock> stateClientMock{std::make_unique<StrictMock<api::AppStateClientMock>>()};
    std::unique_ptr<api::StateFactoryMock> factoryMock{std::make_unique<StrictMock<api::StateFactoryMock>>()};
    std::unique_ptr<api::StateMock> stateInitMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::StateMock> stateRunMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::StateMock> stateTermMock = std::make_unique<StrictMock<api::StateMock>>();
    std::unique_ptr<api::ComponentClientMock> compStateMock = std::make_unique<StrictMock<api::ComponentClientMock>>();
    std::unique_ptr<AppBaseMock> appBaseMock = std::make_unique<StrictMock<AppBaseMock>>();

    api::StateFactoryMock* factoryPtr = factoryMock.get();
    api::AppStateClientMock* stateClientMockPtr = stateClientMock.get();
    AppBaseMock* appBaseMockPtr = appBaseMock.get();

    AdaptiveApp app{std::move(factoryMock), std::move(stateClientMock),
                std::move(compStateMock), std::move(appBaseMock)};
};

TEST_F(AppTest, Should_TransitToInitState)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();
}

TEST_F(AppTest, Should_TransitToRunState)
{

    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*stateInitMock, leave());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();

    EXPECT_CALL(*stateRunMock, enter());
    EXPECT_CALL(*factoryPtr,
                createRun(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

    app.run();
}

TEST_F(AppTest, Should_TransitToTerminateState)
{
    EXPECT_CALL(*stateInitMock, enter());
    EXPECT_CALL(*stateInitMock, leave());
    EXPECT_CALL(*factoryPtr,
                createInit(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateInitMock))));

    app.init();

    EXPECT_CALL(*stateTermMock, enter());
    EXPECT_CALL(*factoryPtr,
                createShutDown(Ref(app)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

    app.terminate();
}

TEST_F(AppTest, shouldReturnExpectMeanWhenSensorDataRead)
{
    EXPECT_CALL(*appBaseMockPtr, mean());
    app.mean();
}

