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
  ExecutionManager::ExecutionManager initEm(
    const std::vector<MachineState>& machineStates,
    const std::map<MachineState, std::vector<ProcessInfo>>& appsForState
  )
  {
    setupManifestData(machineStates, appsForState);
    return ExecutionManager::ExecutionManager{
          std::move(manifestMock),
          std::move(applicationHandler),
          std::move(client)};
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
  const std::string wrongMachineState{"WrongMachineState"};
  const std::string testState{"TestState"};
  const std::vector<ProcessInfo> emptyAvailableApps;
  const std::vector<StartupOption> emptyOptions;
  const std::string firstState{"First"};
  const std::string secondState{"Second"};
  const std::string suspendState{"Suspend"};
  const std::vector<MachineState> transitionStates =
    {firstState, secondState, suspendState};
  const ProcessInfo app{"app", "app", emptyOptions};
  const ProcessInfo additionalApp{"addApp", "addApp", emptyOptions};
};

TEST_F(ExecutionManagerTest,
  ShouldSucceedToGetMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  auto em = initEm({testState}, {{testState, emptyAvailableApps}});

  EXPECT_EQ(
    em.setMachineState(testState),
    StateError::K_SUCCESS
  );
  EXPECT_EQ(em.getMachineState(),
    testState);
}

TEST_F(ExecutionManagerTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
{
  const std::string emptyState{""};

  auto em = initEm({testState}, {{testState, emptyAvailableApps}});

  ASSERT_EQ(
    emptyState,
    em.getMachineState()
  );
}

TEST_F(ExecutionManagerTest, ShouldFailToSetInvalidMachineState)
{
  auto em = initEm({testState}, {{testState, emptyAvailableApps}});

  EXPECT_NE(
    em.setMachineState(wrongMachineState),
    StateError::K_SUCCESS
  );
}

TEST_F(ExecutionManagerTest, ShouldSuccessfullyReportWhenNoSetStateOccured)
{
  auto em = initEm({testState}, {{testState, {app}}});

  em.reportApplicationState(appId, app.processName, AppState::RUNNING);
}

TEST_F(ExecutionManagerTest, ShouldFailToSetSameMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  auto em = initEm({testState}, {{testState, emptyAvailableApps}});
  em.setMachineState(testState);

  EXPECT_NE(
    em.setMachineState(testState),
    StateError::K_SUCCESS);

  EXPECT_EQ(em.getMachineState(),
    testState);
}

TEST_F(ExecutionManagerTest, ShouldTransitToNextStateWhenNoAppInBoth)
{
  auto em = initEm(transitionStates, {});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS)).Times(2);
  em.setMachineState(firstState);
  em.setMachineState(secondState);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldStartAppAndTransitToNextState)
{
  auto em = initEm(transitionStates,
    {{firstState, {}}, {secondState, {app}}});

  {
    InSequence seq;

    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, startProcess(app.processName));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }
  em.setMachineState(firstState);

  em.setMachineState(secondState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldKillAppAndTransitToNextState)
{
  auto em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, emptyAvailableApps}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app.processName))
        .WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(app.processName));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);

  em.setMachineState(secondState);
  em.reportApplicationState(appId, app.processName, AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest,
  ShouldKillOneAppStartAnotherAndTransitToNextState)
{
  auto em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, {additionalApp}}});


  {
    InSequence seq;

    EXPECT_CALL(*pAppHandler, startProcess(app.processName)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(app.processName));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp.processName))
      .WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }
  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);
  em.setMachineState(secondState);
  em.reportApplicationState(appId, app.processName, AppState::SHUTTINGDOWN);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::INITIALIZING);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::RUNNING);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldNotKillAppToTransitState)
{
  auto em = initEm(transitionStates,
    {{firstState, {app}}, {secondState, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app.processName))
        .WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }
  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);
  em.setMachineState(secondState);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldKillTwoAppsToTransitToNextState)
{
  auto em = initEm(transitionStates,
    {{firstState, {app, additionalApp}}, {secondState, emptyAvailableApps}});

  {
    InSequence seq;
  EXPECT_CALL(*pAppHandler, startProcess(additionalApp.processName))
    .WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pAppHandler, startProcess(app.processName))
        .WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(additionalApp.processName));
    EXPECT_CALL(*pAppHandler, killProcess(app.processName));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::INITIALIZING);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::RUNNING);

  em.setMachineState(secondState);
  em.reportApplicationState(appId, app.processName, AppState::SHUTTINGDOWN);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em.getMachineState(),
    secondState
  );
}

TEST_F(ExecutionManagerTest, ShouldNotKillAndTransitToSuspendState)
{
  auto em = initEm(transitionStates,
    {{firstState, {app}}, {suspendState, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app.processName))
        .WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::INITIALIZING);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);

  em.setMachineState(suspendState);
  em.reportApplicationState(appId, app.processName, AppState::SUSPEND);

  ASSERT_EQ(
    em.getMachineState(),
    suspendState
  );
}

TEST_F(ExecutionManagerTest, ShouldKillAndTransitToSuspendState)
{
  auto em = initEm(transitionStates,
  {
    {firstState, {app, additionalApp}},
    {suspendState, {app}},
    {secondState, {}}
  });

  {
    InSequence seq;

  EXPECT_CALL(*pAppHandler, startProcess(additionalApp.processName));
  EXPECT_CALL(*pAppHandler, startProcess(app.processName));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  EXPECT_CALL(*pAppHandler, killProcess(additionalApp.processName));
  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));

  }

  em.setMachineState(firstState);
  em.reportApplicationState(appId, app.processName, AppState::RUNNING);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::RUNNING);

  em.setMachineState(suspendState);
  em.reportApplicationState(appId, app.processName, AppState::SUSPEND);
  em.reportApplicationState(additionalAppId, additionalApp.processName,
                            AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em.getMachineState(),
    suspendState
  );
}
