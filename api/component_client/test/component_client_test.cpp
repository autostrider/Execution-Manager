#include <component_client.h>
#include <constants.hpp>
#include <logger.hpp>

#include <iostream>
#include <thread>
#include <unistd.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <execution_management.capnp.h>
#include <kj/exception.c++>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

using api::ComponentClient;
using ComponentClientReturnType = ::StateManagement::ComponentClientReturnType;

struct TestData
{
  std::string component;
  std::string state;
  ComponentClientReturnType status;

  size_t registerComponentCallCount{0};
  size_t getComponentStateCallCount{0};
  size_t confirmComponentStateCallCount{0};
  bool isTimeouted{false};
};

class ExecutionManagementTestServer
  : public ExecutionManagement::Server
{
public:
  explicit ExecutionManagementTestServer
          (TestData& data)
    : m_data(data)
  {}

private:
  using StateError = ::MachineStateManagement::StateError;

  ::kj::Promise<void>
  registerComponent(RegisterComponentContext context) override
  {
    ++m_data.registerComponentCallCount;

    context.getParams().getComponent().cStr();
    
    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_defaultDelay));
    }
    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  getComponentState(GetComponentStateContext context) override
  {
    ++m_data.getComponentStateCallCount;
    m_data.component = context.getParams().getComponent().cStr();
    
    context.getResults().setState(m_data.state);
    context.getResults().setResult(ComponentClientReturnType::K_SUCCESS);

    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_defaultDelay));
    }
    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  confirmComponentState(ConfirmComponentStateContext context) override
  {
    try
    {
      ++m_data.confirmComponentStateCallCount;
      m_data.component = context.getParams().getComponent().cStr();
      m_data.state = context.getParams().getState().cStr();
      m_data.status = context.getParams().getStatus();
    }
    catch(...){LOG << "ERRRRRROR";}
    
    return kj::READY_NOW;
  }

private:
  TestData& m_data;
  static const uint32_t m_defaultDelay;
};

const uint32_t ExecutionManagementTestServer::m_defaultDelay = 800;

class ComponentClientTest
: public ::testing::Test
{
  protected:
  
  ComponentClientTest() {}
  virtual ~ComponentClientTest() noexcept(true) {}

  virtual void SetUp()
  {
    unlink(EM_SOCKET_NAME.c_str());
  }

  virtual void TearDoun()
  {
		unlink(EM_SOCKET_NAME.c_str());
  }

  const uint32_t defaultTimeout{666};

  TestData testData;
  api::StateUpdateMode mode = api::StateUpdateMode::kPoll;
  std::string componentName = "TestName";
  ExecutionManagementTestServer emServer {testData};
  
  const std::string socketName{IPC_PROTOCOL + EM_SOCKET_NAME};
  capnp::EzRpcServer server{kj::heap<ExecutionManagementTestServer>(testData), socketName}; 

  ComponentClient cc {componentName, mode};
};

TEST_F(ComponentClientTest, ShouldSucceedToSetStateUpdateHandlerState)
{
  std::function<void(std::string const&)> f;

	const auto result = cc.SetStateUpdateHandler(f);

  EXPECT_EQ(result, ComponentClientReturnType::K_SUCCESS);
  EXPECT_EQ(testData.registerComponentCallCount, 1);
}

TEST_F(ComponentClientTest, ShouldSucceedToGetComponentClientState)
{
  testData.state = "TestComponentState";
  std::string state;
  
	const auto result = cc.GetComponentState(state);

  EXPECT_EQ(result, ComponentClientReturnType::K_SUCCESS);
  EXPECT_EQ(testData.getComponentStateCallCount, 1);
  EXPECT_EQ(testData.state, state);
}

TEST_F(ComponentClientTest, ShouldSucceedToConfirmComponentState)
{
  testData.state;
  testData.status;
  std::string state = "TestComponentState";
  ComponentClientReturnType status = ComponentClientReturnType::K_SUCCESS;

  cc.ConfirmComponentState(state, status);

  EXPECT_EQ(testData.confirmComponentStateCallCount, 1);
  EXPECT_EQ(testData.component, componentName);
  EXPECT_EQ(testData.state, state);
  EXPECT_EQ(testData.status, status);
}

TEST_F(ComponentClientTest, ShouldFailToConfirmComponentState)
{
  testData.state;
  testData.status;
  std::string state = std::string(100000000, 'a');
  ComponentClientReturnType status = ComponentClientReturnType::K_SUCCESS;

  EXPECT_DEATH_IF_SUPPORTED(cc.ConfirmComponentState(state, status), "");
}