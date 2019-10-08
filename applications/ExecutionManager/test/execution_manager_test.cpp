#include "execution_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/application_handler_mock.hpp>
#include <mocks/execution_manager_client_mock.hpp>
#include <mocks/manifest_reader_mock.hpp>
#include <mocks/os_interface_mock.hpp>

#include <iostream>
#include "gtest/gtest.h"


using namespace ExecutionManager;

using namespace ::testing;

class ExecutionManagerTest : public ::testing::Test
{
protected:
  std::unique_ptr<ExecutionManager::ExecutionManager> initEm(
    const std::vector<MachineState>& machineStates,
    const std::map<MachineState, std::vector<ProcessInfo>>& appsForState
  )
  {
    setupManifestData(machineStates, appsForState);
    return std::make_unique<ExecutionManager::ExecutionManager>(
          std::move(manifestMock),
          std::move(applicationHandler),
          std::move(client));
  }

  void setupManifestData(const std::vector<MachineState>& machineStates,
    const std::map<MachineState, std::vector<ProcessInfo>>& appsForState)
  {
    ON_CALL(*manifestMock, getMachineStates())
      .WillByDefault(Return(machineStates));

    ON_CALL(*manifestMock, getStatesSupportedByApplication())
      .WillByDefault(Return(appsForState));
  }

  std::unique_ptr<ManifestReaderMock> manifestMock =
    std::make_unique<NiceMock<ManifestReaderMock>>();
  std::unique_ptr<ApplicationHandlerMock> applicationHandler =
    std::make_unique<StrictMock<ApplicationHandlerMock>>();
  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClientMock> client =
    std::make_unique<StrictMock<ExecutionManagerClient::ExecutionManagerClientMock>>();

  const int appId{1};
  const int additionalAppId{2};
  ExecutionManagerClient::ExecutionManagerClientMock* pClient = client.get();
  ApplicationHandlerMock* pAppHandler = applicationHandler.get();
  const pid_t defaultProcessId {666};
  const std::string wrongMachineState{"WrongMachineState"};
  const std::string defaultMsmName{"TestMSM"};
  const std::string testState{"TestState"};
  const std::vector<ProcessInfo> emptyAvailableApps;
  const std::vector<StartupOption> emptyOptions;
  const std::string firstState{"First"};
  const std::string secondState{"Second"};
  const std::vector<MachineState> transitionStates =
    {firstState, secondState};
  const ProcessInfo app{"app", "app", emptyOptions};
  const ProcessInfo additionalApp{"addApp", "addApp", emptyOptions};
};

TEST_F(ExecutionManagerTest,
  ShouldSucceedToGetMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  const auto& em = initEm({testState}, {{testState, emptyAvailableApps}});
  EXPECT_EQ(
    em->setMachineState(testState),
    StateError::K_SUCCESS
  );
  EXPECT_EQ(em->getMachineState(),
    testState);
}

TEST_F(ExecutionManagerTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
{
  const std::string emptyState{""};

  const auto& em = initEm({testState}, {{testState, emptyAvailableApps}});
  ASSERT_EQ(
    emptyState,
    em->getMachineState()
  );
}

TEST_F(ExecutionManagerTest, ShouldFailToSetInvalidMachineState)
{
  const auto& em = initEm({testState}, {{testState, emptyAvailableApps}});
  EXPECT_NE(
    em->setMachineState(wrongMachineState),
    StateError::K_SUCCESS
  );
}

TEST_F(ExecutionManagerTest, ShouldSuccessfullyReportWhenNoSetStateOccured)
{
  const auto& em = initEm({testState}, {{testState, {app}}});

  em->reportApplicationState(appId, AppState::RUNNING);
}

TEST_F(ExecutionManagerTest, ShouldFailToSetSameMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  const auto& em = initEm({testState}, {{testState, emptyAvailableApps}});
  em->setMachineState(testState);

  auto result = em->setMachineState(testState);

  EXPECT_NE(
    em->setMachineState(testState),
    StateError::K_SUCCESS);

  EXPECT_EQ(em->getMachineState(),
    testState);
}

TEST_F(ExecutionManagerTest, ShouldTransitToNextStateWhenNoAppInBoth)
{
  const auto& em = initEm(transitionStates, {});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(firstState);
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldStartAppAndTransitToNextState)
{
  const auto& em = initEm(transitionStates,
    {{firstState, {}}, {secondState, {app}}});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(firstState);

  EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);
  em->reportApplicationState(appId, AppState::RUNNING);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldKillAppAndTransitToNextState)
{
  const auto& em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, emptyAvailableApps}});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
  em->setMachineState(firstState);
  em->reportApplicationState(appId, AppState::RUNNING);
  EXPECT_CALL(*pAppHandler, killProcess(appId));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);
  em->reportApplicationState(appId, AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest,
  ShouldKillOneAppStartAnotherAndTransitToNextState)
{
  const auto& em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, {additionalApp}}});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
  em->setMachineState(firstState);
  em->reportApplicationState(appId, AppState::RUNNING);
  EXPECT_CALL(*pAppHandler, killProcess(appId));
  EXPECT_CALL(*pAppHandler, startProcess(additionalApp))
    .WillOnce(Return(additionalAppId));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);
  em->reportApplicationState(appId, AppState::SHUTTINGDOWN);
  em->reportApplicationState(additionalAppId, AppState::RUNNING);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldNotKillAppToTransitState)
{
  const auto& em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, {app}}});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
  em->setMachineState(firstState);
  em->reportApplicationState(appId, AppState::RUNNING);

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldKillTwoAppsToTransitToNextState)
{
  const auto& em = initEm(transitionStates,
    {{firstState, {app, additionalApp}}, {secondState, emptyAvailableApps}});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
  EXPECT_CALL(*pAppHandler, startProcess(additionalApp))
    .WillOnce(Return(additionalAppId));

  em->setMachineState(firstState);
  em->reportApplicationState(appId, AppState::RUNNING);
  em->reportApplicationState(additionalAppId, AppState::RUNNING);

  EXPECT_CALL(*pAppHandler, killProcess(appId));
  EXPECT_CALL(*pAppHandler, killProcess(additionalAppId));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  em->setMachineState(secondState);
  em->reportApplicationState(appId, AppState::SHUTTINGDOWN);
  em->reportApplicationState(additionalAppId, AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );
}