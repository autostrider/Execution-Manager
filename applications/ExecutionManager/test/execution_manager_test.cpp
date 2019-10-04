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

class ExecutionManagerIpcTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    setupManifestData({testState}, {{testState, emptyAvailableApps}});
  }

  std::unique_ptr<ExecutionManager::ExecutionManager> initEm()
  {
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
  std::unique_ptr<ExecutionManager::ExecutionManager> em;

  const pid_t defaultProcessId {666};
  const std::string wrongMachineState{"WrongMachineState"};
  const std::string defaultMsmName{"TestMSM"};
  const std::string testState{"TestState"};
  const std::vector<ProcessInfo> emptyAvailableApps;
  const std::vector<StartupOption> emptyOptions;
};

class IpcStateTransitionsTest : public ExecutionManagerIpcTest
{
protected:
  void SetUp() override
  {
    std::vector<MachineState> availableStates =
      {firstState, secondState};

    setupManifestData(availableStates, {
      {firstState, {additionalApp}},
      {secondState, {app, additionalApp}}});

    EXPECT_CALL(*client, confirm(StateError::K_SUCCESS)).Times(2);
  }

  const std::string firstState{"First"};
  const std::string secondState{"Second"};
  const ProcessInfo app{"app", "app", emptyOptions};
  const ProcessInfo additionalApp{"addApp", "addApp", emptyOptions};
};

TEST_F(ExecutionManagerIpcTest,
  ShouldSucceedToGetMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  em = initEm();
  EXPECT_EQ(
    em->setMachineState(testState),
    StateError::K_SUCCESS
  );
  EXPECT_EQ(em->getMachineState(),
    testState);
}

TEST_F(ExecutionManagerIpcTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
{
  const std::string emptyState{""};

  em = initEm();
  ASSERT_EQ(
    emptyState,
    em->getMachineState()
  );
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetInvalidMachineState)
{
  em = initEm();
  EXPECT_NE(
    em->setMachineState(wrongMachineState),
    StateError::K_SUCCESS
  );
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetSameMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
  em = initEm();
  em->setMachineState(testState);

  auto result = em->setMachineState(testState);

  EXPECT_NE(
    em->setMachineState(testState),
    StateError::K_SUCCESS);

  EXPECT_EQ(em->getMachineState(),
    testState);
}

TEST_F(IpcStateTransitionsTest,
    ShouldStartAndKillApplicationForStateTransitions)
{
  const pid_t appId = 1;
  const pid_t additionalAppId = 2;
  EXPECT_CALL(*applicationHandler, startProcess(app))
    .WillOnce(Return(appId));

  EXPECT_CALL(*applicationHandler, startProcess(additionalApp))
    .WillOnce(Return(additionalAppId));

  EXPECT_CALL(*applicationHandler, killProcess(appId));

  em = initEm();


  em->setMachineState(secondState);
  em->reportApplicationState(appId, AppState::RUNNING);
  em->reportApplicationState(additionalAppId, AppState::RUNNING);

  ASSERT_EQ(
    em->getMachineState(),
    secondState
  );

  em->setMachineState(firstState);
  em->reportApplicationState(appId, AppState::SHUTTINGDOWN);

  ASSERT_EQ(
    em->getMachineState(),
    firstState
  );
}