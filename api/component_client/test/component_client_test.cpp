#include <component_client.h>
#include <constants.hpp>
#include <logger.hpp>

#include <iostream>
#include <thread>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <execution_management.capnp.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

using ComponentClientReturnType = ::StateManagement::ComponentClientReturnType;

struct TestData
{
  std::string component;
  std::string state;

  size_t registerComponentCallCount{0};
  size_t getComponentStateCallCount{0};
  size_t confirmComponentStateCallCount{0};
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
    ++m_data.confirmComponentStateCallCount;
    // m_data.state = context.getParams().getState().cStr();
    // m_data.pid = context.getParams().getPid();

    // context.getResults().setResult(StateError::K_SUCCESS);

    // sendConfirm(StateError::K_SUCCESS);

    if(m_data.isTimeouted)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_defaultDelay));
    }

    return kj::READY_NOW;
  }

//   void sendConfirm(StateError status)
//   {
//     std::thread([&]()
//     {
//       auto ioContext = kj::setupAsyncIo();

//       auto address = ioContext.provider->getNetwork()
//         .parseAddress(IPC_PROTOCOL + MSM_SOCKET_NAME)
//           .wait(ioContext.waitScope);

//       auto connection = address->connect().wait(ioContext.waitScope);

//       capnp::TwoPartyClient client(*connection);
//       auto capability = client.bootstrap()
//         .castAs<MachineStateManagement::MachineStateManager>();

//       auto request = capability.confirmStateTransitionRequest();

//       request.setResult(status);

//       request.send().ignoreResult().wait(ioContext.waitScope);

//     }).join();
//   }
private:
  TestData& m_data;
  static const uint32_t m_defaultDelay;
};