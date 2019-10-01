#include "execution_manager.hpp"
#include <mocks/app_state_client_mock.hpp>
#include <mocks/application_handler_mock.hpp>
#include <mocks/execution_manager_client_mock.hpp>
#include <mocks/manifest_reader_mock.hpp>
#include <mocks/os_interface_mock.hpp>

#include <iostream>

using namespace ExecutionManager;

using ::testing::Return;

class ExecutionManagerIpcTest : public ::testing::Test
{
public:
  void SetUp() override
  {
    manifestMock = std::make_unique<::testing::NiceMock<ManifestReaderMock>>();
    applicationHandler = std::make_unique<::testing::NiceMock<ApplicationHandlerMock>>();
    client = std::make_unique<::testing::NiceMock<ExecutionManagerClientMock>>();

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
  
  std::unique_ptr<IManifestReader> manifestMock;
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

TEST_F(ExecutionManagerIpcTest, ShouldFailToRegisterWhenAlreadyRegistered)
{
  const pid_t anotherProcessId = 999;

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(anotherProcessId, defaultMsmName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedToGetMachineState)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId,
    "run");

  auto result = em->getMachineState(defaultProcessId);

  EXPECT_EQ(result,
    "run");
}

TEST_F(ExecutionManagerIpcTest,
  ShouldSucceedToStartApplicationAndChangeMachineState)
{
  em->setMachineState(defaultProcessId,
    "start");

  auto result = em->setMachineState(defaultProcessId,
    "run");

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    "run");
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToKillApplicationAndChangeMachineState)
{
  em->setMachineState(defaultProcessId,
    "run");

  auto result = em->setMachineState(defaultProcessId,
    "drive");
  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    "drive");
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
  em->setMachineState(defaultProcessId, "start");

  auto result = em->setMachineState(defaultProcessId,
    "start");

  EXPECT_NE(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    "start");
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedToReportApplicationState)
{
  const AppState applicationState = AppState::RUNNING;

  em->reportApplicationState(defaultProcessId, applicationState);
}
