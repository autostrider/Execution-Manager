// #include "adaptive_app.hpp"
// #include "state.hpp"

// #include <iostream>
// #include <atomic>

// #include "gtest/gtest.h"
// #include "gmock/gmock.h"

// using namespace testing;

// using ApplicationState = api::ApplicationStateClient::ApplicationState;

// class IStateFactoryMock : public api::IStateFactory
// {
// public:
//     MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& app),(const));
//     MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& app),(const));
//     MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& app),(const));
// };

// class IStateMock : public api::IState
// {
// public:
//     MOCK_METHOD(void, enter, ());
//     MOCK_METHOD(void, leave, (), (const));
// };

// class StateClientMock : public api::ApplicationStateClientWrapper
// {
// public:
//     MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
// };

// class ComponentClientMock : public api::IComponentClientWrapper
// {
// public:
//     MOCK_METHOD(api::ComponentClientReturnType, SetStateUpdateHandler,
//                 (std::function<void(api::ComponentState const&)> f), (noexcept));

//     MOCK_METHOD(api::ComponentClientReturnType, GetComponentState,
//                 (api::ComponentState& state), (noexcept));

//     MOCK_METHOD(void, ConfirmComponentState,
//                 (api::ComponentState state, api::ComponentClientReturnType status), (noexcept));
// };
// class IAdaptiveAppMock : public AdaptiveApp
// {
// public:
//     IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
//                      std::unique_ptr<api::IApplicationStateClientWrapper> client,
//                      std::unique_ptr<api::IComponentClientWrapper> compClient);

//     MOCK_METHOD(void, init, ());
//     MOCK_METHOD(void, run, ());
//     MOCK_METHOD(void, terminate, ());

//     MOCK_METHOD(void, reportApplicationState,
//                 (api::ApplicationStateClient::ApplicationState state));
//     MOCK_METHOD(void, transitToNextState, (IAdaptiveApp::FactoryFunc nextState));
// };

// IAdaptiveAppMock::IAdaptiveAppMock(std::unique_ptr<api::IStateFactory> factory,
//                                    std::unique_ptr<api::IApplicationStateClientWrapper> client,
//                                    std::unique_ptr<api::IComponentClientWrapper> compClient) :
//     AdaptiveApp(std::move(factory), std::move(client), std::move(compClient))
// {

// }

// class StateTest : public ::testing::Test
// {
// protected:
//     void SetUp() override
//     {
//         stateClientMock = std::make_unique<StateClientMock>();
//         componentClientMock = std::make_unique<ComponentClientMock>();
//         factoryMock = std::make_unique<IStateFactoryMock>();
//         //appMock = new IAdaptiveAppMock{std::move(factoryMock),
//         //        std::move(stateClientMock),
//          //       std::move(componentClientMock)};
//     }

//     void TearDown() override
//     {
//         //delete appMock;
//     }

//     std::unique_ptr<StateClientMock> stateClientMock{nullptr};
//     std::unique_ptr<ComponentClientMock> componentClientMock{nullptr};
//     std::unique_ptr<IStateFactoryMock> factoryMock{nullptr};
//     IAdaptiveAppMock* appMock{nullptr};
//     StateFactory factory;
// };


// TEST_F(StateTest, Should_InitCallEnter)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

//     std::unique_ptr<Init> state = std::make_unique<Init>(*appMock);
//     state->enter();
//     delete appMock;
// }

// TEST_F(StateTest, Should_RunCallEnterRecvKOn)
// {
//     api::ComponentState expectedState = api::ComponentStatesToString.at(api::ComponentStates::kOn);
//     EXPECT_CALL(*componentClientMock, GetComponentState(_))
//             .WillOnce(DoAll(SetArgReferee<0>(expectedState) ,Return(api::ComponentClientReturnType::kSuccess)));
//     EXPECT_CALL(*componentClientMock, ConfirmComponentState(_, _)).WillOnce(Return());

//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<::Run> state = std::make_unique<::Run>(*appMock);
//     state->enter();

//     delete appMock;
// }

// TEST_F(StateTest, Should_RunCallEnterRecvKOff)
// {
//     api::ComponentState expectedState = api::ComponentStatesToString.at(api::ComponentStates::kOff);
//     EXPECT_CALL(*componentClientMock, GetComponentState(_))
//             .WillOnce(DoAll(SetArgReferee<0>(expectedState) ,Return(api::ComponentClientReturnType::kSuccess)));
//     EXPECT_CALL(*componentClientMock, ConfirmComponentState(_, _)).WillOnce(Return());

//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());
//     std::unique_ptr<::Run> state = std::make_unique<::Run>(*appMock);
//     state->enter();

//     delete appMock;
// }

// TEST_F(StateTest, Should_RunCallEnterRecvInvalidState)
// {
//     api::ComponentState expectedState = "invalidState";
//     EXPECT_CALL(*componentClientMock, GetComponentState(_))
//             .WillOnce(DoAll(SetArgReferee<0>(expectedState) ,Return(api::ComponentClientReturnType::kSuccess)));
//     EXPECT_CALL(*componentClientMock, ConfirmComponentState(_, _)).WillOnce(Return());

//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<::Run> state = std::make_unique<::Run>(*appMock);
//     state->enter();

//     delete appMock;
// }
// TEST_F(StateTest, Should_InitCallLeave)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

//     std::unique_ptr<Init> state = std::make_unique<Init>(*appMock);
//     state->leave();

//     delete appMock;
// }

// TEST_F(StateTest, Should_TerminateCallEnter)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     EXPECT_CALL(*appMock, reportApplicationState(_)).WillOnce(Return());

//     std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*appMock);
//     state->enter();

//     delete appMock;
// }

// TEST_F(StateTest, Should_TerminateCallLeave)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*appMock);
//     state->leave();

//     delete appMock;
// }

// TEST_F(StateTest, Should_CreateInit)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<api::IState> expectedState = std::make_unique<Init>(*appMock);
//     std::unique_ptr<api::IState> createdState = factory.createInit(*appMock);

//     bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
//     ASSERT_TRUE(result);

//     delete appMock;
// }

// TEST_F(StateTest, Should_CreateRun)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<api::IState> expectedState = std::make_unique<::Run>(*appMock);
//     std::unique_ptr<api::IState> createdState = factory.createRun(*appMock);

//     bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
//     ASSERT_TRUE(result);

//     delete appMock;
// }

// TEST_F(StateTest, Should_CreateTerminate)
// {
//     IAdaptiveAppMock* appMock = new IAdaptiveAppMock{std::move(factoryMock),
//             std::move(stateClientMock),
//             std::move(componentClientMock)};

//     std::unique_ptr<api::IState> expectedState = std::make_unique<ShutDown>(*appMock);
//     std::unique_ptr<api::IState> createdState = factory.createShutDown(*appMock);

//     bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
//     ASSERT_TRUE(result);

//     delete appMock;
// }
