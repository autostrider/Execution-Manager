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
    em = initEm();
  }

  std::unique_ptr<ExecutionManager::ExecutionManager> initEm()
  {
    return std::make_unique<ExecutionManager::ExecutionManager>(
          std::move(manifestMock),
          std::move(applicationHandler),
          std::move(client));
  }

  void setupManifestData(std::vector<MachineState> machineStates,
    std::map<MachineState, std::vector<ProcessInfo>> appsForState)
  {
    ON_CALL(*manifestMock, getMachineStates())
      .WillByDefault(Return(machineStates));

    ON_CALL(*manifestMock, getStatesSupportedByApplication())
      .WillByDefault(Return(appsForState));
  }

  void TearDown() override
  {
    em.reset();
  }

  std::unique_ptr<ManifestReaderMock> manifestMock =
    std::make_unique<NiceMock<ManifestReaderMock>>();
  std::unique_ptr<ApplicationHandlerMock> applicationHandler =
    std::make_unique<StrictMock<ApplicationHandlerMock>>();
  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client =
    std::make_unique<NiceMock<ExecutionManagerClientMock>>();
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
      {firstState, emptyAvailableApps},
      {secondState, {app}}});
  }

  const std::string firstState{"First"};
  const std::string secondState{"Second"};
  const ProcessInfo app{ "app", "app", emptyOptions};
};

TEST_F(ExecutionManagerIpcTest, FirstRegistrationShouldSucceed)
{
  bool result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceededWhenSameMsc)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailWhenEmptyNewMsm)
{
  const std::string emptyName;
  auto result = em->registerMachineStateClient(defaultProcessId, emptyName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest,
  ShouldDiscardOtherRegisterWhenAlreadyRegistered)
{
  const pid_t anotherProcessId = 999;

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(122, defaultMsmName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest,
  ShouldSucceedToGetMachineState)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result = em->setMachineState(defaultProcessId,
    testState);

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    testState);
}

TEST_F(ExecutionManagerIpcTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
{
  const std::string emptyState{""};
  auto result = em->getMachineState(defaultProcessId);

  ASSERT_EQ(emptyState, result);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetInvalidMachineState)
{
  auto result = em->setMachineState(defaultProcessId, wrongMachineState);

  EXPECT_NE(result, StateError::K_SUCCESS);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetSameMachineState)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, testState);

  auto result = em->setMachineState(defaultProcessId,
    testState);

  EXPECT_NE(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    testState);
}

TEST_F(IpcStateTransitionsTest,
    ShouldStartAndKillApplicationForStateTransitions)
{
  const pid_t appId = 1;
  EXPECT_CALL(*applicationHandler, startProcess(app))
    .WillOnce(Return(appId));

  EXPECT_CALL(*applicationHandler, killProcess(appId));

  em = initEm();

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, secondState);
  em->reportApplicationState(appId, AppState::RUNNING);

  auto result = em->getMachineState(defaultProcessId);
  ASSERT_EQ(result, secondState);

  em->setMachineState(defaultProcessId, firstState);
  em->reportApplicationState(appId, AppState::SHUTTINGDOWN);

  result = em->getMachineState(defaultProcessId);
  ASSERT_EQ(result, firstState);
}