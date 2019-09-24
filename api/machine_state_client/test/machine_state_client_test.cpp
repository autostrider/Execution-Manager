#include "gtest/gtest.h"
#include <execution_management.capnp.h>
#include <machine_state_client.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>

using api::MachineStateClient;

struct TestData
{
  pid_t pid;
  std::string appName;
  std::string state;

  size_t registerCallCount{0};
  size_t getMachineStateCallCount{0};
  size_t setMachineStateCallCount{0};
  bool isTimeouted{false};
};

class ExecutionManagementTestServer
  : public ExecutionManagement::Server
{
public:
  explicit
  ExecutionManagementTestServer
  (TestData& data)
  : m_data(data)
  {}
private:
  using StateError = ::MachineStateManagement::StateError;

  ::kj::Promise<void>
  register_(RegisterContext context) override
  {
    ++m_data.registerCallCount;
    m_data.appName = context.getParams().getAppName();
    m_data.pid = context.getParams().getPid();

    context.getResults().setResult(StateError::K_SUCCESS);

    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override
  {
    ++m_data.getMachineStateCallCount;
    m_data.pid = context.getParams().getPid();
    context.getResults().setState(m_data.state);

    context.getResults().setResult(StateError::K_SUCCESS);

    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override
  {
    ++m_data.setMachineStateCallCount;
    m_data.state = context.getParams().getState().cStr();
    m_data.pid = context.getParams().getPid();

    context.getResults().setResult(StateError::K_SUCCESS);

    sendConfirm(StateError::K_SUCCESS);

    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return kj::READY_NOW;
  }

  void sendConfirm(StateError status)
  {
    std::thread([&]()
    {
      auto ioContext = kj::setupAsyncIo();

      auto address = ioContext.provider->getNetwork()
        .parseAddress("unix:/tmp/machine_management")
          .wait(ioContext.waitScope);

      auto connection = address->connect().wait(ioContext.waitScope);

      capnp::TwoPartyClient client(*connection);

      auto capability = client.bootstrap()
        .castAs<MachineStateManagement::MachineStateManager>();

      auto request = capability.confirmStateTransitionRequest();
      request.setResult(status);

      request.send().ignoreResult().wait(ioContext.waitScope);
    }).join();
  }
private:

  TestData& m_data;
};

class MachineStateClientTest
: public ::testing::Test
{
public:
  MachineStateClientTest()
  {}

  ~MachineStateClientTest() noexcept
  {}

  void SetUp()
  {
    serverThread = std::thread([&]()
    {
      auto ioContext = kj::setupAsyncIo();

      std::cout << "Server thread enter" << std::endl;
      capnp::TwoPartyServer server(
        kj::heap<ExecutionManagementTestServer>(testData));

      auto address = ioContext.provider->getNetwork()
          .parseAddress(std::string{em_address})
            .wait(ioContext.waitScope);

      auto listener = address->listen();
      auto listenPromise = server.listen(*listener);

      auto exitPaf = kj::newPromiseAndFulfiller<void>();
      auto exitPromise = listenPromise.exclusiveJoin(kj::mv(exitPaf.promise));

      listenFulfiller = kj::mv(exitPaf.fulfiller);

      *executor.lockExclusive() = kj::getCurrentThreadExecutor();

      exitPromise.wait(ioContext.waitScope);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    msc = std::make_unique<MachineStateClient>(em_address);

    std::cout << "SetUp exit" << std::endl;
  }

  void TearDown()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    const kj::Executor* exec;
    {
      auto lock = executor.lockExclusive();
      lock.wait([&](kj::Maybe<const kj::Executor&> value)
      {
        return value != nullptr;
      });
      exec = &KJ_ASSERT_NONNULL(*lock);
    }

    std::cout << "Before execute" << std::endl;
    exec->executeSync([&]()
    {
      listenFulfiller->fulfill();
    });

    *executor.lockExclusive() = nullptr;
    serverThread.join();
    msc.reset();

    testData.pid = -1;
    testData.appName = "";
    testData.state = "";

    testData.registerCallCount = 0;
    testData.getMachineStateCallCount = 0;
    testData.setMachineStateCallCount = 0;

    testData.isTimeouted = false;

    unlink("/tmp/machine_management");
    unlink("/tmp/msc_test");
  }

  std::thread serverThread;

  const std::string applicationName{"TestMSC"};
  const uint32_t defaultTimeout{666};

  std::unique_ptr<MachineStateClient> msc;

  TestData testData;
  kj::Own<kj::PromiseFulfiller<void>> listenFulfiller;
  kj::MutexGuarded<kj::Maybe<const kj::Executor&>> executor;
  const std::string em_address{"unix:/tmp/msc_test"};
};

TEST_F(MachineStateClientTest, ShouldSucceedToRegister)
{
  const auto result = msc->Register(applicationName, defaultTimeout);

  EXPECT_EQ(result, MachineStateClient::StateError::K_SUCCESS);
  EXPECT_EQ(testData.registerCallCount, 1);
  EXPECT_EQ(testData.appName, applicationName);
  EXPECT_EQ(testData.pid, getpid());
}

TEST_F(MachineStateClientTest, ShouldSucceedToGetMachineState)
{
  testData.state = "TestMachineState";
  std::string state;
  const auto result = msc->GetMachineState(defaultTimeout, state);

  EXPECT_EQ(result, MachineStateClient::StateError::K_SUCCESS);
  EXPECT_EQ(testData.getMachineStateCallCount, 1);
  EXPECT_EQ(testData.state, state);
  EXPECT_EQ(testData.pid, getpid());
}

TEST_F(MachineStateClientTest, ShouldSucceedToSetMachineState)
{
  msc->Register(applicationName, defaultTimeout);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::string state = "TestMachineState";
  const auto result = msc->SetMachineState(state, defaultTimeout);

  EXPECT_EQ(result, MachineStateClient::StateError::K_SUCCESS);
  EXPECT_EQ(testData.setMachineStateCallCount, 1);
  EXPECT_EQ(testData.state, state);
  EXPECT_EQ(testData.pid, getpid());
}

TEST_F(MachineStateClientTest, ShouldTimeoutOnRegister)
{
  testData.isTimeouted = true;
  const auto result = msc->Register(applicationName, defaultTimeout);

  EXPECT_EQ(result, MachineStateClient::StateError::K_TIMEOUT);
  EXPECT_EQ(testData.registerCallCount, 1);
}

TEST_F(MachineStateClientTest, ShouldTimeoutOnGettingMachineState)
{
  testData.isTimeouted = true;
  std::string state;

  const auto result = msc->GetMachineState(defaultTimeout, state);

  EXPECT_EQ(result, MachineStateClient::StateError::K_TIMEOUT);
  EXPECT_EQ(state, "");
  EXPECT_EQ(testData.getMachineStateCallCount, 1);
}

TEST_F(MachineStateClientTest, ShouldTimeoutOnSettingMachineState)
{
  testData.isTimeouted = true;
  const auto result = msc->SetMachineState("TestMachineState", defaultTimeout);

  EXPECT_EQ(result, MachineStateClient::StateError::K_TIMEOUT);
  EXPECT_EQ(testData.setMachineStateCallCount, 1);
}