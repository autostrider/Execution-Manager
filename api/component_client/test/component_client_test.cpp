#include "component_client.h"
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
    context.getParams().getComponent().cStr();
  
    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  getComponentState(GetComponentStateContext context) override
  {
    m_data.component = context.getParams().getComponent().cStr();
    
    context.getResults().setState(m_data.state);
    context.getResults().setResult(ComponentClientReturnType::K_SUCCESS);

    return kj::READY_NOW;
  }

  ::kj::Promise<void>
  confirmComponentState(ConfirmComponentStateContext context) override
  {
      m_data.component = context.getParams().getComponent().cStr();
      m_data.state = context.getParams().getState().cStr();
      m_data.status = context.getParams().getStatus();

    return kj::READY_NOW;
  }

private:
  TestData& m_data;
  static const uint32_t m_defaultDelay;
};

class ComponentClientTest
: public ::testing::Test
{
  protected:
  virtual ~ComponentClientTest() noexcept(true) {}

  void SetUp() override
  {
    unlink(EM_SOCKET_NAME.c_str());
  }

  void TearDown() override
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

TEST_F(ComponentClientTest, ShouldSucceedToSetStateUpdateHandler)
{
  std::function<void(std::string const&)> f;

	const auto result = cc.SetStateUpdateHandler(f);

  EXPECT_EQ(result, ComponentClientReturnType::K_SUCCESS);
}

TEST_F(ComponentClientTest, ShouldSucceedToGetComponentClientState)
{
  testData.state = "TestComponentState";
  std::string state;
  
	const auto result = cc.GetComponentState(state);

  EXPECT_EQ(result, ComponentClientReturnType::K_SUCCESS);
  EXPECT_EQ(testData.state, state);
}

TEST_F(ComponentClientTest, ShouldSucceedToConfirmComponentState)
{
  std::string state = "TestComponentState";
  ComponentClientReturnType status = ComponentClientReturnType::K_SUCCESS;

  cc.ConfirmComponentState(state, status);

  EXPECT_EQ(testData.component, componentName);
  EXPECT_EQ(testData.state, state);
  EXPECT_EQ(testData.status, status);
}

TEST_F(ComponentClientTest, ShouldSuccessfullyConfirmComponentStateWithInvalidStatus)
{
  std::string state = "TestComponentState";
  ComponentClientReturnType status = ComponentClientReturnType::K_INVALID;

  cc.ConfirmComponentState(state, status);

  EXPECT_EQ(testData.component, componentName);
  EXPECT_EQ(testData.state, state);
  EXPECT_EQ(testData.status, status);
}