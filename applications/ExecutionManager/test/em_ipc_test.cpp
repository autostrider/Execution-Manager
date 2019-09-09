#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <string>
#include "execution_manager.hpp"

using namespace ExecutionManager;

using ::testing::Return;

class ManifestReaderMock : public ManifestReader
{
public:
  MOCK_METHOD0(getStatesSupportedByApplication,
    std::map<MachineState, std::vector<ProcessName>>());

  MOCK_METHOD0(getMachineStates, std::vector<MachineState>());
};

class ExecutionManagerIpcTest : public ::testing::Test
{
public:
  void SetUp()
  {
    auto manifestMock = std::make_unique<ManifestReaderMock>();

    EXPECT_CALL(*manifestMock, getStatesSupportedByApplication())
      .WillOnce(Return(
        std::map<MachineState, std::vector<ProcessName>>{}
      ));

    EXPECT_CALL(*manifestMock, getMachineStates())
      .WillOnce(Return(
        std::vector<MachineState>{"Startup", "Running", "Shutdown"}
      ));

    em =
      std::make_unique<ExecutionManager::ExecutionManager>
        (std::move(manifestMock));
  }

  void TearDown()
  {
    em.reset();
  }

  std::unique_ptr<ExecutionManager::ExecutionManager> em;

  const pid_t defaultProcessId {666};
  const std::string defaultMsmName{"TestMSM"};
};

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedWhenEmptyMsc)
{
  auto result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedWhenSameMsc)
{
  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(defaultProcessId, defaultMsmName);

  EXPECT_TRUE(result);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldFailWhenEmptyNewMsc)
{
  const std::string emptyString = "";
  auto result = em->registerMachineStateClient(defaultProcessId, emptyString);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldFailToRegisterWhenAlredyRegistered)
{
  const pid_t anotherProcessId = 999;

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result =
    em->registerMachineStateClient(anotherProcessId, defaultMsmName);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToGetMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, machineState);

  auto result = em->getMachineState(defaultProcessId);

  EXPECT_EQ(result, machineState);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToSetMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_TRUE(result);
  EXPECT_EQ(em->getMachineState(defaultProcessId), machineState);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldFailToSetInvalidMachineState)
{
  const std::string machineState = "WrongMachineState";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_FALSE(result);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldFailToSetSameMachineState)
{
  const std::string machineState = "Running";

  em->registerMachineStateClient(defaultProcessId, defaultMsmName);
  em->setMachineState(defaultProcessId, machineState);
  auto result = em->setMachineState(defaultProcessId, machineState);

  EXPECT_FALSE(result);
  EXPECT_EQ(em->getMachineState(defaultProcessId), machineState);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToReportApplicationState)
{
  const AppState applicationState = AppState::RUNNING;

  em->reportApplicationState(defaultProcessId, applicationState);
}