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

  void expectGetAndConfirmComponentState(ExecutionManager::ExecutionManager&,
                                         ComponentTestData&,
                                         const std::string&);
};

void
ExecutionManagerTest::expectGetAndConfirmComponentState(ExecutionManager::ExecutionManager& em,
                                                        ComponentTestData& compTestData,
                                                        const std::string& expectState)
{
  em.getComponentState(compTestData.component, compTestData.state);
  em.confirmComponentState(compTestData.component, compTestData.state, compTestData.status);

  EXPECT_EQ(expectState, compTestData.state);
}

TEST_F(ExecutionManagerTest, ShouldSucceedToSetStartingUpMachineState)
{
  auto em = initEm(transitionStates, {});

  EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  
  em.start();
  em.setMachineState(MACHINE_STATE_STARTUP);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_STARTUP
  );
}

TEST_F(ExecutionManagerTest, ShouldSucceedToGetMachineState)
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

  em.reportApplicationState(appId, AppState::kRunning);
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

    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
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
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingDown);

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
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp))
      .WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }
  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);
  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingDown);
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
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
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
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(additionalAppId));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_STARTUP);
  em.reportApplicationState(appId, AppState::kRunning);
  em.reportApplicationState(additionalAppId, AppState::kRunning);

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.reportApplicationState(appId, AppState::kShuttingDown);
  em.reportApplicationState(additionalAppId, AppState::kShuttingDown);

  ASSERT_EQ(
    em.getMachineState(),
    MACHINE_STATE_RUNNING
  );
}

TEST_F(ExecutionManagerTest, ShouldTransitToSuspendState)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  em.setMachineState(MACHINE_STATE_SUSPEND);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldKillAndTransitToSuspendState)
{
  ComponentTestData componentTestData = {};

  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app}},
                    {MACHINE_STATE_SUSPEND, {}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);
  
  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.reportApplicationState(appId, AppState::kShuttingDown);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldKillOneProcessAndTransitToSuspendState)
{
  ComponentTestData componentTestData = {};
  ComponentTestData componentTestDataForAdditionalApp = {};

  componentTestData.component = app.processName;
  componentTestDataForAdditionalApp.component = additionalApp.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app, additionalApp}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp)).WillOnce(Return(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(additionalAppId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  em.registerComponent(componentTestDataForAdditionalApp.component);
  em.reportApplicationState(additionalAppId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestDataForAdditionalApp, COMPONENT_STATE_ON);
  
  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.reportApplicationState(additionalAppId, AppState::kShuttingDown);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldStartAndTransitToSuspendState)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {}},
                    {MACHINE_STATE_SUSPEND, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);

  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  ASSERT_EQ(em.getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldRegisterComponent)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm(transitionStates, {});
  
  em.reportApplicationState(appId, AppState::kRunning);
  em.registerComponent(componentTestData.component);
  em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(COMPONENT_STATE_ON, componentTestData.state);
}

TEST_F(ExecutionManagerTest, ShouldNotGetComponentStateWhenComponentNotRegistered)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_SUSPEND}, {{MACHINE_STATE_SUSPEND, {app}}});
  
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.reportApplicationState(appId, AppState::kRunning);

  auto result = em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(componentTestData.state, "");
  EXPECT_EQ(result, ComponentClientReturnType::K_GENERAL_ERROR);
}

TEST_F(ExecutionManagerTest, ShouldGetComponentStateWhenComponentRegistered)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);

  auto result = em.getComponentState(componentTestData.component, componentTestData.state);

  EXPECT_EQ(COMPONENT_STATE_ON, componentTestData.state);
  EXPECT_EQ(result, ComponentClientReturnType::K_SUCCESS);
}

TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentState)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_RUNNING);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  EXPECT_EQ(componentTestData.status, ComponentClientReturnType::K_SUCCESS);
}

TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentStateForSuspend)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app.processName;

  auto em = initEm({MACHINE_STATE_SUSPEND}, {{MACHINE_STATE_SUSPEND, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app)).WillOnce(Return(appId));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  em.setMachineState(MACHINE_STATE_SUSPEND);
  em.registerComponent(componentTestData.component);
  em.reportApplicationState(appId, AppState::kRunning);
  expectGetAndConfirmComponentState(em, componentTestData, COMPONENT_STATE_ON);

  EXPECT_EQ(componentTestData.status, ComponentClientReturnType::K_SUCCESS);
}