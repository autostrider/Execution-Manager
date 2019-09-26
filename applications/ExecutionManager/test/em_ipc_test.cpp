#include "execution_manager.hpp"

#include <i_manifest_reader.hpp>
#include <i_execution_manager_client.hpp>

#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ExecutionManager;

using ::testing::Return;

class ManifestReaderMock : public IManifestReader
{
public:
  MOCK_METHOD0(getStatesSupportedByApplication,
    std::map<MachineState, std::vector<ProcessInfo>>());

  MOCK_METHOD0(getMachineStates, std::vector<MachineState>());
};

class ExecutionManagerClientMock :
  public ExecutionManagerClient::IExecutionManagerClient
{
public:
  MOCK_METHOD(void, confirm, (StateError status));
};

class ApplicationHandlerMock : public IApplicationHandler
{
public:
  MOCK_METHOD(pid_t, startProcess, (const ProcessInfo& application));
  MOCK_METHOD(void, killProcess, (pid_t processId));
};

class ExecutionManagerIpcTest : public ::testing::Test
{
public:
  void SetUp() override
  {
    manifestMock = std::make_unique<ManifestReaderMock>();
    applicationHandler = std::make_unique<ApplicationHandlerMock>();
    client = std::make_unique<ExecutionManagerClientMock>();
    
    EXPECT_CALL(*manifestMock, getStatesSupportedByApplication())
      .WillOnce(Return(
        std::map<MachineState, std::vector<ProcessInfo>>{}
      ));

    EXPECT_CALL(*manifestMock, getMachineStates())
      .WillOnce(Return(
        std::vector<MachineState>{"Startup", "Running", "Shutdown"}
      ));

    em = std::make_unique<ExecutionManager::ExecutionManager>(std::move(manifestMock),
                                                              std::move(applicationHandler),
                                                              std::move(client));
  }

  void TearDown() override
  {
    em.reset();
  }
  
  std::unique_ptr<ManifestReaderMock> manifestMock;
  std::unique_ptr<IApplicationHandler> applicationHandler;
  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client;
  std::unique_ptr<ExecutionManager::ExecutionManager> em;

  const pid_t defaultProcessId {666};
  const std::string defaultMsmName{"TestMSM"};
};

TEST_F(ExecutionManagerIpcTest, ShouldSucceedWhenEmptyMsc)
{
  bool result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedWhenSameMsc)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailWhenEmptyNewMsc)
{
  const std::string emptyName = "";
  auto result = em->registerMachineStateClient(defaultProcessId, emptyName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToRegisterWhenAlredyRegistered)
{
  const pid_t anotherProcessId = 999;

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(anotherProcessId, defaultMsmName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedToGetMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, machineState);

  auto result = em->getMachineState(defaultProcessId);

  EXPECT_EQ(result, machineState);
}


TEST_F(ExecutionManagerIpcTest, ShouldSucceedToSetMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId), machineState);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetInvalidMachineState)
{
  const std::string machineState = "WrongMachineState";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_NE(result, StateError::K_SUCCESS);
}

TEST_F(ExecutionManagerIpcTest, ShouldFailToSetSameMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, machineState);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_NE(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId), machineState);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedToReportApplicationState)
{
  const AppState applicationState = AppState::RUNNING;

  em->reportApplicationState(defaultProcessId, applicationState);
}