// #include <constants.hpp>
// #include <execution_manager.hpp>

// #include <application_handler_mock.hpp>
// #include <app_state_client_mock.hpp>
// #include <execution_manager_client_mock.hpp>
// #include <manifest_reader_mock.hpp>
// #include <os_interface_mock.hpp>

// #include <future>

// #include "gtest/gtest.h"


// using namespace ExecutionManager;
// using namespace std::chrono_literals;
// using namespace constants;
// using namespace ::testing;


// class ExecutionManagerTest : public ::testing::Test
// {
// protected:
//     std::unique_ptr<ExecutionManager::ExecutionManager> initEm(
//             const std::vector<MachineState>& machineStates,
//             const std::map<MachineState, std::set<ProcName>>& appsForState
//             )
//     {
//         setupManifestData(machineStates, appsForState);
//         return std::make_unique<ExecutionManager::ExecutionManager>(
//                     std::move(manifestMock),
//                     std::move(applicationHandler),
//                     std::move(client));
//     }

//     void setupManifestData(const std::vector<MachineState>& machineStates,
//                            const std::map<MachineState, std::set<ProcName>>& appsForState)
//     {
//         ON_CALL(*manifestMock, getMachineStates())
//                 .WillByDefault(Return(machineStates));

//         ON_CALL(*manifestMock, getStatesSupportedByApplication())
//                 .WillByDefault(Return(appsForState));
//     }

//     std::unique_ptr<ManifestReaderMock> manifestMock =
//             std::make_unique<NiceMock<ManifestReaderMock>>();
//     std::unique_ptr<ApplicationHandlerMock> applicationHandler =
//             std::make_unique<StrictMock<ApplicationHandlerMock>>();
//     std::unique_ptr<ExecutionManagerClientMock> client =
//             std::make_unique<StrictMock<ExecutionManagerClientMock>>();

//     const std::chrono::seconds oneSecond{1};
//     const int appId{1};
//     const int additionalAppId{2};
//     ExecutionManagerClientMock* pClient = client.get();
//     ApplicationHandlerMock* pAppHandler = applicationHandler.get();
//     const std::string wrongMachineState{"WrongMachineState"};
//     const std::string testState{"TestState"};

//     const std::set<ProcName> emptyAvailableApps{};

//     const std::string app{"app_app"};
//     const std::string additionalApp{"additionalApp_additionalApp"};
//     const std::string criticalApp{"msmApp"};
//     const std::vector<MachineState> transitionStates =
//     {MACHINE_STATE_STARTUP, MACHINE_STATE_RUNNING, MACHINE_STATE_LIVING,
//      MACHINE_STATE_SUSPEND, MACHINE_STATE_SHUTTINGDOWN};

// };


// TEST_F(ExecutionManagerTest, ShouldSucceedToSetStartingUpMachineState)
// {
//     auto em = initEm({MACHINE_STATE_STARTUP}, {{MACHINE_STATE_STARTUP, {criticalApp}}});

//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess)).Times(1);

//     auto res = std::async(std::launch::async, [&]()
//     {
//         EXPECT_CALL(*pAppHandler, startProcess(_));
//         EXPECT_CALL(*pAppHandler, isActiveProcess(_)).WillRepeatedly(Return(true));
//         return em->setMachineState(MACHINE_STATE_STARTUP);
//     });

//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(criticalApp, AppState::kRunning);

//     ASSERT_EQ(res.get(), StateError::kSuccess);
//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_STARTUP
//                 );
// }

// TEST_F(ExecutionManagerTest,
//        ShouldNotToSetStartingUpMachineStateWhenCriticalAppIsNotRunning)
// {
//     auto em = initEm({MACHINE_STATE_STARTUP}, {{MACHINE_STATE_STARTUP, {criticalApp}}});

//     EXPECT_CALL(*pAppHandler, startProcess(_));
//     EXPECT_CALL(*pAppHandler, isActiveProcess(_)).WillRepeatedly(Return(false));
//     ASSERT_EQ(em->setMachineState(MACHINE_STATE_STARTUP), StateError::kInvalidRequest);
// }

// TEST_F(ExecutionManagerTest,
//        ShouldSucceedToGetMachineState)
// {
//     EXPECT_CALL(*client, confirm(StateError::kSuccess));
//     auto em = initEm({testState}, {{testState, emptyAvailableApps}});

//     EXPECT_EQ(
//                 em->setMachineState(testState),
//                 StateError::kSuccess
//                 );
//     EXPECT_EQ(em->getMachineState(),
//               testState);
// }


// TEST_F(ExecutionManagerTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
// {
//     const std::string emptyState{""};

//     auto em = initEm({testState}, {{testState, emptyAvailableApps}});

//     ASSERT_EQ(
//                 emptyState,
//                 em->getMachineState()
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldFailToSetInvalidMachineState)
// {
//     auto em = initEm({testState}, {{testState, emptyAvailableApps}});

//     EXPECT_NE(
//                 em->setMachineState(wrongMachineState),
//                 StateError::kSuccess
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldSuccessfullyReportWhenNoSetStateOccured)
// {
//     auto em = initEm({testState}, {{testState, {app}}});

//     em->reportApplicationState(app, AppState::kRunning);
// }

// TEST_F(ExecutionManagerTest, ShouldFailToSetSameMachineState)
// {
//     EXPECT_CALL(*client, confirm(StateError::kSuccess));
//     auto em = initEm({testState}, {{testState, emptyAvailableApps}});
//     em->setMachineState(testState);

//     EXPECT_NE(
//                 em->setMachineState(testState),
//                 StateError::kSuccess);

//     EXPECT_EQ(em->getMachineState(),
//               testState);
// }

// TEST_F(ExecutionManagerTest, ShouldTransitToNextStateWhenNoAppInBoth)
// {
//     auto em = initEm(transitionStates, {});

//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess)).Times(2);

//     em->setMachineState(MACHINE_STATE_RUNNING);
//     em->setMachineState(MACHINE_STATE_LIVING);

//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_LIVING
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldTransitToNextStateAndStartApp)
// {
//     auto em = initEm(transitionStates,
//     {{MACHINE_STATE_RUNNING, {}},
//      {MACHINE_STATE_LIVING, {app}}});
//     {
//         InSequence seq;

//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//         EXPECT_CALL(*pAppHandler, startProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     }
//     em->setMachineState(MACHINE_STATE_RUNNING);

//     auto res = std::async(std::launch::async, [&]()
//     {
//         return em->setMachineState(MACHINE_STATE_LIVING);
//     });

//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kRunning);

//     ASSERT_EQ(res.get(), StateError::kSuccess);
//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_LIVING
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldKillAppAndTransitToNextState)
// {
//     auto em = initEm(transitionStates,
//     {{MACHINE_STATE_LIVING, {app}},
//      {MACHINE_STATE_SHUTTINGDOWN, emptyAvailableApps}});
//     {
//         InSequence seq;
//         EXPECT_CALL(*pAppHandler, startProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//         EXPECT_CALL(*pAppHandler, killProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     }

//     auto res = std::async(std::launch::async, [&]()
//     {
//         return em->setMachineState(MACHINE_STATE_LIVING);
//     });

//     std::this_thread::sleep_for(oneSecond);

//     em->reportApplicationState(app, AppState::kRunning);
//     ASSERT_EQ(res.get(), StateError::kSuccess);


//     auto res2 = std::async(std::launch::async, [&]()
//     {
//         return em->setMachineState(MACHINE_STATE_SHUTTINGDOWN);
//     });

//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kShuttingDown);

//     ASSERT_EQ(res2.get(), StateError::kSuccess);
//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_SHUTTINGDOWN
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldKillOneAppStartAnotherAndTransitToNextState)
// {
//     auto em = initEm(transitionStates,
//     {{MACHINE_STATE_RUNNING, {app}},
//      {MACHINE_STATE_LIVING, {additionalApp}}});
//     {
//         InSequence seq;

//         EXPECT_CALL(*pAppHandler, startProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//         EXPECT_CALL(*pAppHandler, killProcess(app));
//         EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     }

//     auto res = std::async(std::launch::async, [&]()
//     {
//         return  em->setMachineState(MACHINE_STATE_RUNNING);
//     });

//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kRunning);
//     ASSERT_EQ(res.get(), StateError::kSuccess);

//     auto res2 = std::async(std::launch::async, [&]()
//     {
//         return  em->setMachineState(MACHINE_STATE_LIVING);
//     });

//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kShuttingDown);
//     em->reportApplicationState(additionalApp, AppState::kRunning);

//     ASSERT_EQ(res2.get(), StateError::kSuccess);
//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_LIVING
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldNotKillAppToTransitState)
// {
//     auto em = initEm(transitionStates,
//     {{MACHINE_STATE_RUNNING, {app}},
//      {MACHINE_STATE_LIVING, {app}}});
//     {
//         InSequence seq;
//         EXPECT_CALL(*pAppHandler, startProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     }

//     auto res = std::async(std::launch::async, [&]()
//     {
//         return  em->setMachineState(MACHINE_STATE_RUNNING);
//     });
//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kRunning);

//     ASSERT_EQ(res.get(), StateError::kSuccess);

//     em->setMachineState(MACHINE_STATE_LIVING);

//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_LIVING
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldKillTwoAppsToTransitToNextState)
// {
//     auto em = initEm(transitionStates,
//     {{MACHINE_STATE_RUNNING, {app, additionalApp}},
//      {MACHINE_STATE_LIVING, {}}});
//     {
//         InSequence seq;
//         EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
//         EXPECT_CALL(*pAppHandler, startProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//         EXPECT_CALL(*pAppHandler, killProcess(additionalApp));
//         EXPECT_CALL(*pAppHandler, killProcess(app));
//         EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     }

//     auto res = std::async(std::launch::async, [&]()
//     {
//         return em->setMachineState(MACHINE_STATE_RUNNING);
//     });
//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kRunning);
//     em->reportApplicationState(additionalApp, AppState::kRunning);

//     ASSERT_EQ(res.get(), StateError::kSuccess);

//     auto res2 = std::async(std::launch::async, [&]()
//     {
//         return em->setMachineState(MACHINE_STATE_LIVING);
//     });
//     std::this_thread::sleep_for(oneSecond);
//     em->reportApplicationState(app, AppState::kShuttingDown);
//     em->reportApplicationState(additionalApp, AppState::kShuttingDown);

//     ASSERT_EQ(res2.get(), StateError::kSuccess);
//     ASSERT_EQ(
//                 em->getMachineState(),
//                 MACHINE_STATE_LIVING
//                 );
// }

// TEST_F(ExecutionManagerTest, ShouldTransitToSuspendState)
// {

//   auto em = initEm(transitionStates,
//                    {{MACHINE_STATE_RUNNING, {app}},
//                     {MACHINE_STATE_SUSPEND, {app}}});
//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);
//   ASSERT_EQ(res.get(), StateError::kSuccess);

//   auto res2 = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);

//   ASSERT_EQ(res2.get(), StateError::kSuccess);
//   ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
// }

// TEST_F(ExecutionManagerTest, ShouldKillAndTransitToSuspendState)
// {
//   auto em = initEm(transitionStates,
//                    {{MACHINE_STATE_RUNNING, {app}},
//                     {MACHINE_STATE_SUSPEND, {}}});
//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     EXPECT_CALL(*pAppHandler, killProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);
  
//   ASSERT_EQ(res.get(), StateError::kSuccess);

//   auto res2 = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kShuttingDown);
//   ASSERT_EQ(res2.get(), StateError::kSuccess);
//   ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
// }

// TEST_F(ExecutionManagerTest, ShouldKillOneProcessAndTransitToSuspendState)
// {
//   auto em = initEm(transitionStates,
//                    {{MACHINE_STATE_RUNNING, {app, additionalApp}},
//                     {MACHINE_STATE_SUSPEND, {app}}});
//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(additionalApp));
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     EXPECT_CALL(*pAppHandler, killProcess(additionalApp));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   em->reportApplicationState(additionalApp, AppState::kRunning);
//   ASSERT_EQ(res.get(), StateError::kSuccess);

//   auto res2 = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(additionalApp, AppState::kShuttingDown);

//   ASSERT_EQ(res2.get(), StateError::kSuccess);
//   ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
// }

// TEST_F(ExecutionManagerTest, ShouldTransitToSuspendStateAndStartApp)
// {
//   auto em = initEm(transitionStates,
//                    {{MACHINE_STATE_RUNNING, {}},
//                     {MACHINE_STATE_SUSPEND, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   ASSERT_EQ(em->setMachineState(MACHINE_STATE_RUNNING), StateError::kSuccess);

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
//   ASSERT_EQ(em->getMachineState(), MACHINE_STATE_SUSPEND);
// }

// TEST_F(ExecutionManagerTest, ShouldRegisterComponent)
// {
//   auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }
  
//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldNotGetComponentStateWhenComponentNotRegistered)
// {
//   auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});
  
//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldGetComponentStateWhenComponentRegistered)
// {
//   auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentState)
// {
//   auto em = initEm({MACHINE_STATE_RUNNING}, {{MACHINE_STATE_RUNNING, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldConfirmSuccessComponentStateForSuspend)
// {
//   auto em = initEm({MACHINE_STATE_SUSPEND}, {{MACHINE_STATE_SUSPEND, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);

//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldSetMachineStateWithApplicationThatHasEventMode)
// {

//   auto em = initEm({MACHINE_STATE_SUSPEND, MACHINE_STATE_RUNNING},
//                    {{MACHINE_STATE_SUSPEND, {app}},
//                     {MACHINE_STATE_RUNNING, {app}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return em->setMachineState(MACHINE_STATE_SUSPEND);
//   });
//   std::this_thread::sleep_for(oneSecond);

//   em->reportApplicationState(app, AppState::kRunning);

//   auto res1 = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);
  
//   ASSERT_EQ(res.get(), StateError::kSuccess);
//   ASSERT_EQ(res1.get(), StateError::kSuccess);
// }

// TEST_F(ExecutionManagerTest, ShouldNotSetMachineState)
// {
//   auto em = initEm({},
//                    {{MACHINE_STATE_RUNNING, {additionalApp}}});

//   StateError result = em->setMachineState(MACHINE_STATE_RUNNING);

//   ASSERT_EQ(result, StateError::kInvalidState);
// }

// TEST_F(ExecutionManagerTest, ShouldNotTransitToInvalidNextState)
// {
//   std::string invalidState = "invalidState";

//   auto em = initEm(transitionStates,
//                    {{MACHINE_STATE_RUNNING, {app}},
//                     {invalidState, {additionalApp}}});

//   {
//     InSequence seq;
//     EXPECT_CALL(*pAppHandler, startProcess(app));
//     EXPECT_CALL(*pClient, confirm(StateError::kSuccess));
//   }

//   auto res = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(MACHINE_STATE_RUNNING);
//   });
//   std::this_thread::sleep_for(oneSecond);

//   em->reportApplicationState(app, AppState::kRunning);

//   ASSERT_EQ(res.get(), StateError::kSuccess);

//   auto res1 = std::async(std::launch::async, [&]()
//   {
//       return  em->setMachineState(invalidState);
//   });

//   ASSERT_EQ(res1.get(), StateError::kInvalidState);
// }
