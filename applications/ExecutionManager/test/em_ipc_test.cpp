#include "manifest_reader.hpp"
#include "execution_manager.hpp"

#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ExecutionManager;

using ::testing::Return;

class ManifestReaderMock : public ManifestReader
{
public:
  MOCK_METHOD0(getStatesSupportedByApplication,
    std::map<MachineState, std::vector<ProcessInfo>>());

  MOCK_METHOD0(getMachineStates, std::vector<MachineState>());
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
  void SetUp()
  {
    manifestMock = std::make_unique<ManifestReaderMock>();
    applicationHandler = std::make_unique<ApplicationHandlerMock>();
    client = std::make_unique<ExecutionManagerClient::ExecutionManagerClient>();
    
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

  void TearDown()
  {
    em.reset();
  }
  
  std::unique_ptr<ManifestReaderMock> manifestMock{nullptr};
  std::unique_ptr<IApplicationHandler> applicationHandler{nullptr};
  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client{nullptr};
  std::unique_ptr<ExecutionManager::ExecutionManager> em;

  const pid_t defaultProcessId {666};
  const std::string defaultMsmName{"TestMSM"};
};

TEST_F(ExecutionManagerIpcTest, ShouldSucceedWhenEmptyMsc)
{
  auto result =
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
  const std::string emptyString = "";
  auto result = em->registerMachineStateClient(defaultProcessId, emptyString);

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

  EXPECT_TRUE(result);
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