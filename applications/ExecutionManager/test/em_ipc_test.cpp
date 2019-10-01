#include "execution_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/application_handler_mock.hpp>
#include <mocks/execution_manager_client_mock.hpp>
#include <mocks/manifest_reader_mock.hpp>
#include <mocks/os_interface_mock.hpp>

#include <iostream>
#include "gtest/gtest.h"

using namespace ExecutionManager;

using ::testing::Return;
using ::testing::NiceMock;
using ::testing::StrictMock;

class ExecutionManagerIpcTest : public ::testing::Test
{
public:
  void SetUp() override
  {
    manifestMock = std::make_unique<NiceMock<ManifestReaderMock>>();
    applicationHandler = std::make_unique<NiceMock<ApplicationHandlerMock>>();
    client = std::make_unique<NiceMock<ExecutionManagerClientMock>>();

    ON_CALL(*manifestMock, getMachineStates())
      .WillByDefault(Return(
        std::vector<MachineState>{"Startup", "Running"}
      ));

     EXPECT_CALL(*manifestMock, getStatesSupportedByApplication())
      .WillOnce(Return(
        std::map<MachineState, std::vector<ProcessInfo>>{
         { "Startup",  {}},
         {"Running", {ProcessInfo{"app2", "app2", emptyOptions}}}
        }
      ));
    

    em = std::make_unique<ExecutionManager::ExecutionManager>(
          std::move(manifestMock),
          std::move(applicationHandler),
          std::move(client));

    em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  }

  void TearDown() override
  {
    em.reset();
  }
  
  std::vector<StartupOption> emptyOptions;
  std::unique_ptr<ManifestReaderMock> manifestMock;
  std::unique_ptr<IApplicationHandler> applicationHandler;
  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client;
  std::unique_ptr<ExecutionManager::ExecutionManager> em;

  const pid_t defaultProcessId {666};
  const std::string defaultMsmName{"TestMSM"};
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

////////////////////////////////////////////////

TEST_F(ExecutionManagerIpcTest,
  ShouldSucceedToGetMachineState)
{
  auto result = em->setMachineState(defaultProcessId,
    "Startup");

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    "Startup");
}

TEST_F(ExecutionManagerIpcTest, ShouldReturnEmptyStateWhenNoSetStateOccured)
{
  const std::string emptyState{""};
  auto result = em->getMachineState(defaultProcessId);

  ASSERT_EQ(emptyState, result);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetInvalidMachineState)
{
  const std::string machineState{"WrongMachineState"};

  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_NE(result, StateError::K_SUCCESS);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetSameMachineState)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, "Startup");

  auto result = em->setMachineState(defaultProcessId,
    "Startup");

  EXPECT_NE(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    "Startup");
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToKillApplicationAndChangeMachineState)
{
  em->setMachineState(defaultProcessId, "run");

  auto result = em->setMachineState(defaultProcessId, "Startup");
  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId), "Startup");
}

// TEST_F(ExecutionManagerIpcTest,
//     ShouldStartApplicationForCreating)
// {
//   em->registerMachineStateClient(defaultProcessId, defaultMsmName);
//   em->setMachineState(defaultProcessId, "Running");

//   EXPECT_CALL(*applicationHandler, startApplication)
// }