#include "execution_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/application_handler_mock.hpp>
#include <mocks/execution_manager_client_mock.hpp>
#include <mocks/manifest_reader_mock.hpp>
#include <mocks/os_interface_mock.hpp>
#include <constants.hpp>

#include <iostream>
#include "gtest/gtest.h"


using namespace ExecutionManager;

using namespace ::testing;

struct ComponentTestData
{
  std::string component;
  std::string state;
  ComponentClientReturnType status;
};


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

  ExecutionManagerClient::ExecutionManagerClientMock* pClient = client.get();
  ApplicationHandlerMock* pAppHandler = applicationHandler.get();

  const std::string wrongMachineState{"WrongMachineState"};
  const std::string testState{"TestState"};

  const std::vector<ProcessInfo> emptyAvailableApps;
  const std::vector<StartupOption> emptyOptions;

  const ProcessInfo app{"app", "app", emptyOptions};
  const ProcessInfo additionalApp{"addApp", "addApp", emptyOptions};

  const int appId{1};
  const int additionalAppId{2};
  
  const std::vector<MachineState> transitionStates =
    {MACHINE_STATE_STARTUP, MACHINE_STATE_RUNNING, MACHINE_STATE_LIVING, MACHINE_STATE_SUSPEND};

  ComponentTestData componentTestData;

  void confirmComponentState(ExecutionManager::ExecutionManager&,
                             ComponentTestData& componentTestData,
                             std::string);
};

void
ExecutionManagerTest::confirmComponentState(ExecutionManager::ExecutionManager& em,
                                            ComponentTestData& fCompTestData,
                                            std::string test)
{
  em.getComponentState(fCompTestData.component, fCompTestData.state);
  em.confirmComponentState(fCompTestData.component, fCompTestData.state, fCompTestData.status);

  EXPECT_EQ(test, componentTestData.state);
}

TEST_F(ExecutionManagerTest, ShouldSucceedToSetStartingUpMachineState)
{
  auto em = initEm(transitionStates, {});

  EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  
  em.start();
  em.setMachineState(MACHINE_STATE_STARTUP);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_STARTUP
  );
}

TEST_F(ExecutionManagerTest, ShouldSucceedToGetMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::kSuccess));
  auto em = initEm({testState}, {{testState, emptyAvailableApps}});

  EXPECT_EQ(
    em.setMachineState(testState),
    StateError::kSuccess
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
    StateError::kSuccess
  );
}

TEST_F(ExecutionManagerTest, ShouldSuccessfullyReportWhenNoSetStateOccured)
{
  auto em = initEm({testState}, {{testState, {app}}});

  em.reportApplicationState(appId, AppState::kRunning);
}

TEST_F(ExecutionManagerTest, ShouldFailToSetSameMachineState)
{
  EXPECT_CALL(*client, confirm(StateError::kSuccess));
  auto em = initEm({testState}, {{testState, emptyAvailableApps}});
  em.setMachineState(testState);

  EXPECT_NE(
    em.setMachineState(testState),
    StateError::kSuccess);

  EXPECT_EQ(em.getMachineState(),
    testState);
}

TEST_F(ExecutionManagerTest, ShouldTransitToNextStateWhenNoAppInBoth)
{
  auto em = initEm(transitionStates, {});

  EXPECT_CALL(*pClient, confirm(StateError::kSuccess)).Times(2);
  em.setMachineState(MACHINE_STATE_STARTUP);
  em.setMachineState(MACHINE_STATE_RUNNING);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldStartAppAndTransitToNextState)
{
  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_STARTUP, {}},
                    {MACHINE_STATE_RUNNING, {app}}});
  {
    InSequence seq;

    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }
  em.setMachineState(MACHINE_STATE_STARTUP);

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kRunning);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldKillAppAndTransitToNextState)
{
  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_STARTUP, {app}},
                    {MACHINE_STATE_RUNNING, emptyAvailableApps}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingdown);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest,
  ShouldKillOneAppStartAnotherAndTransitToNextState)
{
  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_STARTUP, {app}},
                    {MACHINE_STATE_RUNNING, {additionalApp}}});
  {
    InSequence seq;

    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp))
      .WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }
  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);
  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingdown);
  em.reportApplicationState(additionalAppId, AppState::kRunning);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldNotKillAppToTransitState)
{
  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_STARTUP, {app}},
                    {MACHINE_STATE_RUNNING, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }
  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);
  em.setMachineState(MACHINE_STATE_RUNNING);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldKillTwoAppsToTransitToNextState)
{
  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_STARTUP, {app, additionalApp}},
                    {MACHINE_STATE_RUNNING, emptyAvailableApps}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp))
      .WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, killProcess(additionalAppId));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);
  em.reportApplicationState(additionalAppId, AppState::kRunning);

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingdown);
  em.reportApplicationState(additionalAppId, AppState::kShuttingdown);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldTransitToSuspendState)
{
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  em.setMachineState(MACHINE_STATE_SUSPEND);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldKillAndTransitToSuspendState)
{
  ComponentTestData componentTestData2;
  componentTestData2.component = additionalApp.processName;

  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app, additionalApp}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp)).WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, killProcess(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  em.registerComponent(componentTestData2.component);
  em.reportApplicationState(additionalAppId, AppState::kRunning);
  confirmComponentState(em, componentTestData2, COMPONENT_STATE_ON);
  
  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.reportApplicationState(additionalAppId, AppState::kShuttingdown);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldStartAndTransitToSuspendState)
{
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {}},
                    {MACHINE_STATE_SUSPEND, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);

  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldRegisterComponent)
{
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates, {});
  
  em.reportApplicationState(appId, AppState::kRunning);
  em.registerComponent(componentTestData.component);
  em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(COMPONENT_STATE_ON, componentTestData.state);
}

TEST_F(ExecutionManagerTest, ShouldNotGetComponentStateWhenComponentNotRegistered)
{
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_SUSPEND}, {{MACHINE_STATE_SUSPEND, {app}}});
  
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.reportApplicationState(appId, AppState::kRunning);

  auto result = em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(componentTestData.state, "");
  EXPECT_EQ(result, ComponentClientReturnType::kInvalid);
}

TEST_F(ExecutionManagerTest, ShouldGetComponentStateWhenComponentRegistered)
{
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);

  auto result = em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(COMPONENT_STATE_ON, componentTestData.state);
  EXPECT_EQ(result, ComponentClientReturnType::kSuccess);
}

TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentState)
{
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  confirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  EXPECT_EQ(componentTestData.status, ComponentClientReturnType::kSuccess);
}
