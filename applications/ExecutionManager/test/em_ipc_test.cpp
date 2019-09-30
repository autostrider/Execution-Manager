#include "execution_manager.hpp"
#include <mocks.hpp>

#include <iostream>

using namespace ExecutionManager;

using ::testing::Return;

class IpcManifestReaderTest : public IManifestReader
{
public:
  std::map<MachineState, std::vector<ProcessInfo>>
  getStatesSupportedByApplication() override
  {
    return
    {
      {"Start", {{"app1", "app1", {}}} },
      {"Running", {{"app1", "app1", {}}, {"app2", "app2", {}}} },
      {"Drive", {{"app2", "app2", {}}} },
    };
  }
  std::vector<MachineState> getMachineStates() override
  {
    return {"Start", "Running", "Drive"};
  }

  static const std::string startMachineState;
  static const std::string runMachineState;
  static const std::string driveMachineState;

  static constexpr int app1PId = 0;
  static constexpr int app2PId = 1;
};

const std::string IpcManifestReaderTest::startMachineState = "Start";
const std::string IpcManifestReaderTest::runMachineState = "Running";
const std::string IpcManifestReaderTest::driveMachineState = "Drive";

class IpcApplicationHandlerTest : public IApplicationHandler
{
public:
  pid_t startProcess(const ProcessInfo &application) override { return pId++; }
  void killProcess(pid_t processId) override {}
private:
  int pId{0};
};

class ExecutionManagerIpcTest : public ::testing::Test
{
public:
  void SetUp() override
  {
    manifestMock = std::make_unique<IpcManifestReaderTest>();
    applicationHandler = std::make_unique<IpcApplicationHandlerTest>();
    client = std::make_unique<ExecutionManagerClientMock>();

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
    IpcManifestReaderTest::runMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app1PId,
    AppState::RUNNING);

  em->reportApplicationState(IpcManifestReaderTest::app2PId,
    AppState::RUNNING);

  auto result = em->getMachineState(defaultProcessId);

  EXPECT_EQ(result,
    IpcManifestReaderTest::runMachineState);
}

TEST_F(ExecutionManagerIpcTest,
  ShouldSucceedToStartApplicationAndChangeMachineState)
{
  em->setMachineState(defaultProcessId,
    IpcManifestReaderTest::startMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app1PId,
                             AppState::RUNNING);

  auto result = em->setMachineState(defaultProcessId,
    IpcManifestReaderTest::runMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app2PId,
                             AppState::RUNNING);

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    IpcManifestReaderTest::runMachineState);
}

TEST_F(ExecutionManagerIpcTest,
       ShouldSucceedToKillApplicationAndChangeMachineState)
{
  em->setMachineState(defaultProcessId,
    IpcManifestReaderTest::runMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app1PId,
                             AppState::RUNNING);

  em->reportApplicationState(IpcManifestReaderTest::app2PId,
                             AppState::RUNNING);

  auto result = em->setMachineState(defaultProcessId,
    IpcManifestReaderTest::driveMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app1PId,
                             AppState::SHUTTINGDOWN);

  EXPECT_EQ(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    IpcManifestReaderTest::driveMachineState);
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
  em->setMachineState(defaultProcessId, IpcManifestReaderTest::runMachineState);

  em->reportApplicationState(IpcManifestReaderTest::app1PId,
                             AppState::RUNNING);

  em->reportApplicationState(IpcManifestReaderTest::app2PId,
                             AppState::RUNNING);

  auto result = em->setMachineState(defaultProcessId,
    IpcManifestReaderTest::runMachineState);

  EXPECT_NE(result, StateError::K_SUCCESS);
  EXPECT_EQ(em->getMachineState(defaultProcessId),
    IpcManifestReaderTest::runMachineState);
}

TEST_F(ExecutionManagerIpcTest, ShouldSucceedToReportApplicationState)
{
  const AppState applicationState = AppState::RUNNING;

  em->reportApplicationState(defaultProcessId, applicationState);
}
