#include "execution_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/application_handler_mock.hpp>
#include <mocks/execution_manager_client_mock.hpp>
#include <mocks/manifest_reader_mock.hpp>
#include <mocks/os_interface_mock.hpp>
#include <constants.hpp>

#include <future>

#include "gtest/gtest.h"


using namespace ExecutionManager;
using namespace std::chrono_literals;
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
    std::unique_ptr<ExecutionManager::ExecutionManager> initEm(
            const std::vector<MachineState>& machineStates,
            const std::map<MachineState, std::set<ProcName>>& appsForState
            )
    {
        setupManifestData(machineStates, appsForState);
        return std::make_unique<ExecutionManager::ExecutionManager>(
                    std::move(manifestMock),
                    std::move(applicationHandler),
                    std::move(client));
    }

    void setupManifestData(const std::vector<MachineState>& machineStates,
                           const std::map<MachineState, std::set<ProcName>>& appsForState)
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

    const std::chrono::seconds oneSecond{1};
    const int appId{1};
    const int additionalAppId{2};
    ExecutionManagerClient::ExecutionManagerClientMock* pClient = client.get();
    ApplicationHandlerMock* pAppHandler = applicationHandler.get();
    const std::string wrongMachineState{"WrongMachineState"};
    const std::string testState{"TestState"};

    const std::set<ProcName> emptyAvailableApps{};

    const std::string app{"app_app"};
    const std::string additionalApp{"additionalApp_additionalApp"};
    const std::string criticalApp{"msmApp"};
    const std::vector<MachineState> transitionStates =
    {MACHINE_STATE_STARTUP, MACHINE_STATE_RUNNING, MACHINE_STATE_LIVING,
     MACHINE_STATE_SUSPEND, MACHINE_STATE_SHUTTINGDOWN};

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
    auto em = initEm({MACHINE_STATE_STARTUP}, {{MACHINE_STATE_STARTUP, {criticalApp}}});

    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS)).Times(1);

    auto res = std::async(std::launch::async, [&]()
    {
        EXPECT_CALL(*pAppHandler, startProcess(_));
        EXPECT_CALL(*pAppHandler, isActiveProcess(_)).WillRepeatedly(Return(true));
        return em->setMachineState(MACHINE_STATE_STARTUP);
    });

    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(criticalApp, AppState::kRunning);

    ASSERT_EQ(res.get(), StateError::K_SUCCESS);
    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_STARTUP
                );
}

TEST_F(ExecutionManagerTest,
       ShouldNotToSetStartingUpMachineStateWhenCriticalAppIsNotRunning)
{
    auto em = initEm({MACHINE_STATE_STARTUP}, {{MACHINE_STATE_STARTUP, {criticalApp}}});

    EXPECT_CALL(*pAppHandler, startProcess(_));
    EXPECT_CALL(*pAppHandler, isActiveProcess(_)).WillRepeatedly(Return(false));
    ASSERT_EQ(em->setMachineState(MACHINE_STATE_STARTUP), StateError::K_INVALID_REQUEST);
}

TEST_F(ExecutionManagerTest,
       ShouldSucceedToGetMachineState)
{
    EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
    auto em = initEm({testState}, {{testState, emptyAvailableApps}});

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

    auto em = initEm({testState}, {{testState, emptyAvailableApps}});

    ASSERT_EQ(
                emptyState,
                em->getMachineState()
                );
}

TEST_F(ExecutionManagerTest, ShouldFailToSetInvalidMachineState)
{
    auto em = initEm({testState}, {{testState, emptyAvailableApps}});

    EXPECT_NE(
                em->setMachineState(wrongMachineState),
                StateError::K_SUCCESS
                );
}

TEST_F(ExecutionManagerTest, ShouldSuccessfullyReportWhenNoSetStateOccured)
{
    auto em = initEm({testState}, {{testState, {app}}});

    em->reportApplicationState(app, AppState::kRunning);
}

TEST_F(ExecutionManagerTest, ShouldFailToSetSameMachineState)
{
    EXPECT_CALL(*client, confirm(StateError::K_SUCCESS));
    auto em = initEm({testState}, {{testState, emptyAvailableApps}});
    em->setMachineState(testState);

    EXPECT_NE(
                em->setMachineState(testState),
                StateError::K_SUCCESS);

    EXPECT_EQ(em->getMachineState(),
              testState);
}

TEST_F(ExecutionManagerTest, ShouldTransitToNextStateWhenNoAppInBoth)
{
    auto em = initEm(transitionStates, {});

    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS)).Times(2);

    em->setMachineState(MACHINE_STATE_RUNNING);
    em->setMachineState(MACHINE_STATE_LIVING);

    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_LIVING
                );
}

TEST_F(ExecutionManagerTest, ShouldTransitToNextStateAndStartApp)
{
    auto em = initEm(transitionStates,
    {{MACHINE_STATE_RUNNING, {}},
     {MACHINE_STATE_LIVING, {app}}});
    {
        InSequence seq;

        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
        EXPECT_CALL(*pAppHandler, startProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    }
    em->setMachineState(MACHINE_STATE_RUNNING);

    auto res = std::async(std::launch::async, [&]()
    {
        return em->setMachineState(MACHINE_STATE_LIVING);
    });

    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kRunning);

    ASSERT_EQ(res.get(), StateError::K_SUCCESS);
    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_LIVING
                );
}

TEST_F(ExecutionManagerTest, ShouldKillAppAndTransitToNextState)
{
    auto em = initEm(transitionStates,
    {{MACHINE_STATE_LIVING, {app}},
     {MACHINE_STATE_SHUTTINGDOWN, emptyAvailableApps}});
    {
        InSequence seq;
        EXPECT_CALL(*pAppHandler, startProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
        EXPECT_CALL(*pAppHandler, killProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    }

    auto res = std::async(std::launch::async, [&]()
    {
        return em->setMachineState(MACHINE_STATE_LIVING);
    });

    std::this_thread::sleep_for(oneSecond);

    em->reportApplicationState(app, AppState::kRunning);
    ASSERT_EQ(res.get(), StateError::K_SUCCESS);


    auto res2 = std::async(std::launch::async, [&]()
    {
        return em->setMachineState(MACHINE_STATE_SHUTTINGDOWN);
    });

    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kShuttingDown);

    ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_SHUTTINGDOWN
                );
}

TEST_F(ExecutionManagerTest, ShouldKillOneAppStartAnotherAndTransitToNextState)
{
    auto em = initEm(transitionStates,
    {{MACHINE_STATE_RUNNING, {app}},
     {MACHINE_STATE_LIVING, {additionalApp}}});
    {
        InSequence seq;

        EXPECT_CALL(*pAppHandler, startProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
        EXPECT_CALL(*pAppHandler, killProcess(app));
        EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    }

    auto res = std::async(std::launch::async, [&]()
    {
        return  em->setMachineState(MACHINE_STATE_RUNNING);
    });

    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kRunning);
    ASSERT_EQ(res.get(), StateError::K_SUCCESS);

    auto res2 = std::async(std::launch::async, [&]()
    {
        return  em->setMachineState(MACHINE_STATE_LIVING);
    });

    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kShuttingDown);
    em->reportApplicationState(additionalApp, AppState::kRunning);

    ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_LIVING
                );
}

TEST_F(ExecutionManagerTest, ShouldNotKillAppToTransitState)
{
    auto em = initEm(transitionStates,
    {{MACHINE_STATE_RUNNING, {app}},
     {MACHINE_STATE_LIVING, {app}}});
    {
        InSequence seq;
        EXPECT_CALL(*pAppHandler, startProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    }

    auto res = std::async(std::launch::async, [&]()
    {
        return  em->setMachineState(MACHINE_STATE_RUNNING);
    });
    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kRunning);

    ASSERT_EQ(res.get(), StateError::K_SUCCESS);

    em->setMachineState(MACHINE_STATE_LIVING);

    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_LIVING
                );
}

TEST_F(ExecutionManagerTest, ShouldKillTwoAppsToTransitToNextState)
{
    auto em = initEm(transitionStates,
    {{MACHINE_STATE_RUNNING, {app, additionalApp}},
     {MACHINE_STATE_LIVING, {}}});
    {
        InSequence seq;
        EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
        EXPECT_CALL(*pAppHandler, startProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
        EXPECT_CALL(*pAppHandler, killProcess(additionalApp));
        EXPECT_CALL(*pAppHandler, killProcess(app));
        EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    }

    auto res = std::async(std::launch::async, [&]()
    {
        return em->setMachineState(MACHINE_STATE_RUNNING);
    });
    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kRunning);
    em->reportApplicationState(additionalApp, AppState::kRunning);

    ASSERT_EQ(res.get(), StateError::K_SUCCESS);

    auto res2 = std::async(std::launch::async, [&]()
    {
        return em->setMachineState(MACHINE_STATE_LIVING);
    });
    std::this_thread::sleep_for(oneSecond);
    em->reportApplicationState(app, AppState::kShuttingDown);
    em->reportApplicationState(additionalApp, AppState::kShuttingDown);

    ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
    ASSERT_EQ(
                em->getMachineState(),
                MACHINE_STATE_LIVING
                );
}

TEST_F(ExecutionManagerTest, ShouldTransitToSuspendState)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_ON);
  ASSERT_EQ(res.get(), StateError::K_SUCCESS);

  auto res2 = std::async(std::launch::async, [&]()
  {
      return em->setMachineState(MACHINE_STATE_SUSPEND);
  });
  std::this_thread::sleep_for(oneSecond);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
  ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldKillAndTransitToSuspendState)
{
  ComponentTestData componentTestData = {};

  componentTestData.component = app;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app}},
                    {MACHINE_STATE_SUSPEND, {}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_ON);
  
  ASSERT_EQ(res.get(), StateError::K_SUCCESS);

  auto res2 = std::async(std::launch::async, [&]()
  {
      return em->setMachineState(MACHINE_STATE_SUSPEND);
  });
  std::this_thread::sleep_for(oneSecond);
  em->reportApplicationState(app, AppState::kShuttingDown);
  ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
  ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldKillOneProcessAndTransitToSuspendState)
{
  ComponentTestData componentTestData = {};
  ComponentTestData componentTestDataForAdditionalApp = {};

  componentTestData.component = app;
  componentTestDataForAdditionalApp.component = additionalApp;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {app, additionalApp}},
                    {MACHINE_STATE_SUSPEND, {app}}});
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, killProcess(additionalApp));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_ON);

  em->registerComponent(componentTestDataForAdditionalApp.component);
  em->reportApplicationState(additionalApp, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestDataForAdditionalApp, COMPONENT_STATE_ON);
  ASSERT_EQ(res.get(), StateError::K_SUCCESS);

  auto res2 = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_SUSPEND);
  });
  std::this_thread::sleep_for(oneSecond);
  em->reportApplicationState(additionalApp, AppState::kShuttingDown);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(res2.get(), StateError::K_SUCCESS);
  ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldTransitToSuspendStateAndStartApp)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm(transitionStates,
                   {{MACHINE_STATE_RUNNING, {}},
                    {MACHINE_STATE_SUSPEND, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  ASSERT_EQ(em->setMachineState(MACHINE_STATE_RUNNING), StateError::K_SUCCESS);

  auto res = std::async(std::launch::async, [&]()
  {
      return em->setMachineState(MACHINE_STATE_SUSPEND);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
}

TEST_F(ExecutionManagerTest, ShouldRegisterComponent)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }
  
  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->reportApplicationState(app, AppState::kRunning);
  em->registerComponent(componentTestData.component);
  em->getComponentState(componentTestData.component, componentTestData.state);

  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(COMPONENT_STATE_ON, componentTestData.state);
}

TEST_F(ExecutionManagerTest, ShouldNotGetComponentStateWhenComponentNotRegistered)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});
  
  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->reportApplicationState(app, AppState::kRunning);

  auto result = em->getComponentState(componentTestData.component, componentTestData.state);
  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(componentTestData.state, "");
  ASSERT_EQ(result, ComponentClientReturnType::K_GENERAL_ERROR);
}

TEST_F(ExecutionManagerTest, ShouldGetComponentStateWhenComponentRegistered)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);

  auto result = em->getComponentState(componentTestData.component, componentTestData.state);

  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(COMPONENT_STATE_ON, componentTestData.state);
  ASSERT_EQ(result, ComponentClientReturnType::K_SUCCESS);
}

TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentState)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_RUNNING);
  });
  std::this_thread::sleep_for(oneSecond);
  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_ON);

  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(componentTestData.status, ComponentClientReturnType::K_SUCCESS);
}

TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentStateForSuspend)
{
  ComponentTestData componentTestData = {};
  componentTestData.component = app;

  auto em = initEm({MACHINE_STATE_SUSPEND}, {{MACHINE_STATE_SUSPEND, {app}}});

  {
    InSequence seq;
    EXPECT_CALL(*pAppHandler, startProcess(app));
    EXPECT_CALL(*pClient, confirm(StateError::K_SUCCESS));
  }

  auto res = std::async(std::launch::async, [&]()
  {
      return  em->setMachineState(MACHINE_STATE_SUSPEND);
  });
  std::this_thread::sleep_for(oneSecond);

  em->registerComponent(componentTestData.component);
  em->reportApplicationState(app, AppState::kRunning);
  expectGetAndConfirmComponentState(*em, componentTestData, COMPONENT_STATE_OFF);

  ASSERT_EQ(res.get(), StateError::K_SUCCESS);
  ASSERT_EQ(componentTestData.status, ComponentClientReturnType::K_SUCCESS);
}

