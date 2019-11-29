#include "component_client.h"
#include <constants.hpp>
#include <logger.hpp>
#include <thread>

namespace api
{
ComponentClient::ComponentClient
(const std::string& componentName, 
 StateUpdateMode mode) noexcept
: m_rpcClient(IPC_PROTOCOL + EM_SOCKET_NAME),
  m_componentName(componentName),
  m_updateMode(mode),
  m_eventPromise{std::promise<ComponentState>()},
  m_serverStopPromise{kj::Promise<void>(nullptr)},
  m_stateManagementCap{m_rpcClient.getMain<StateManagement>()}
{
// ==========================================================================

// ==========================================================================



  auto request = m_stateManagementCap.registerComponentRequest();
  request.setComponent(m_componentName);
  request.setMode(m_updateMode);
  request.send().wait(m_rpcClient.getWaitScope());
}

ComponentClient::~ComponentClient()
{
  if (!m_listenFulfiller.get())
  {
    return;
  }
 
  const kj::Executor* exec;
  {
    auto lock = m_serverExecutor.lockExclusive();
    lock.wait([&](kj::Maybe<const kj::Executor&> value)
    {
      return value != nullptr;
    });

    exec = &KJ_ASSERT_NONNULL(*lock);
  }

  exec->executeSync([&]()
  {
    m_listenFulfiller->fulfill();
  });
}

ComponentClientReturnType
ComponentClient::SetStateUpdateHandler
(std::function<void(ComponentState const&)> f) noexcept
{
  m_stateUpdateHandler = f;

  startServer();

  return ComponentClientReturnType::K_SUCCESS;
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  auto request = m_stateManagementCap.getComponentStateRequest();
  request.setComponent(m_componentName);

  auto result = request.send().wait(m_rpcClient.getWaitScope());
  state = result.getState();

  return result.getResult();
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  auto request = m_stateManagementCap.confirmComponentStateRequest();

  LOG << "############## Sending CONFIRM for " << m_componentName << ", state : " << state << ", status : " << static_cast<uint32_t>(status);
  request.setComponent(m_componentName);
  request.setState(state);
  request.setStatus(status);

  request.send().wait(m_rpcClient.getWaitScope());
 }

// =================================================================================================================================

bool ComponentClient::eventReceived(std::future<ComponentState>& stateFuture)
{
  return stateFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
}

void ComponentClient::checkIfAnyEventsAvailable()
{
  auto stateFuture = m_eventPromise.get_future(); 
   
  if (eventReceived(stateFuture))
  {
    m_stateUpdateHandler(stateFuture.get());
  }

  m_eventPromise = std::promise<ComponentState>();
}

void ComponentClient::startServer()
{
  std::promise<void> startUpPromise;
  ::unlink((COMPONENT_SOCKET_NAME + m_componentName).c_str());

  m_serverThread = kj::heap<kj::Thread>(
  [&]()
  {
    auto io = kj::setupAsyncIo();

    capnp::TwoPartyServer server(kj::heap<ComponentServer>(m_eventPromise));

    auto address = io.provider->getNetwork()
                   .parseAddress(IPC_PROTOCOL + COMPONENT_SOCKET_NAME + m_componentName)
                   .wait(io.waitScope);

    auto listener = address->listen();
    auto listenPromise = server.listen(*listener);

    *m_serverExecutor.lockExclusive() = kj::getCurrentThreadExecutor();

    auto exitPaf = kj::newPromiseAndFulfiller<void>();
    auto exitPromise = listenPromise.exclusiveJoin(kj::mv(exitPaf.promise));
    m_listenFulfiller = kj::mv(exitPaf.fulfiller);

    startUpPromise.set_value();

    exitPromise.wait(io.waitScope);
    LOG << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  STOPPING SERVER !!!!!!!!!!!!!!!!!!!!!! m_componentName = " << m_componentName;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  });

  startUpPromise.get_future().wait();
}

ComponentServer::ComponentServer
  (std::promise<ComponentState>& promise)
  : m_eventPromise(promise)
{
}

::kj::Promise<void>
ComponentServer::setComponentState
(SetComponentStateContext context)
{
  auto state = context.getParams().getState().cStr();

  LOG << "StateUpdate event received, new state : " << state;

  m_eventPromise.set_value(state);

  return kj::READY_NOW;
}

} // namespace api
