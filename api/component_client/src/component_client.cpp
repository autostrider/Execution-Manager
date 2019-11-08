#include "component_client.h"
#include <constants.hpp>
#include <logger.hpp>
#include <thread>

namespace api
{
ComponentClient::ComponentClient
(const std::string &s, StateUpdateMode mode) noexcept
: m_client(IPC_PROTOCOL + EM_SOCKET_NAME), 
  m_componentName(s),
  m_updateMode(mode),
  m_promise(std::promise<ComponentClientReturnType>())
{
  auto cap =
    m_client.getMain<StateManagement>();

  auto request = cap.registerComponentRequest();
  request.setComponent(m_componentName);
  request.setMode(m_updateMode);

  request.send().wait(m_client.getWaitScope());
}

ComponentClientReturnType
ComponentClient::SetStateUpdateHandler
(std::function<void(ComponentState const&)> f) noexcept
{
  m_stateUpdateHandler = f;
  m_serverThread = startServer();

  return ComponentClientReturnType::K_SUCCESS;
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  auto cap =
    m_client.getMain<StateManagement>();

  auto request = cap.getComponentStateRequest();
  request.setComponent(m_componentName);

  auto result = request.send().wait(m_client.getWaitScope());

  state = result.getState();

  return result.getResult();
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  auto cap =
    m_client.getMain<StateManagement>();

  auto request = cap.confirmComponentStateRequest();

  request.setComponent(m_componentName);
  request.setState(state);
  request.setStatus(status);

  request.send().wait(m_client.getWaitScope());
}

// =================================================================================================================================

kj::Own<kj::Thread> ComponentClient::startServer()
{
  std::promise<void> startUpPromise;

  ::unlink((COMPONENT_SOCKET_NAME + m_componentName).c_str());

  auto sThread = kj::heap<kj::Thread>([&]() noexcept
  {
    auto ioContext = kj::setupAsyncIo();

    capnp::TwoPartyServer server(kj::heap<ComponentServer>(m_stateUpdateHandler));

    auto address = ioContext.provider->getNetwork()
                   .parseAddress(IPC_PROTOCOL + COMPONENT_SOCKET_NAME + m_componentName)
                   .wait(ioContext.waitScope);

    auto listener = address->listen();
    auto listenPromise = server.listen(*listener);

    *m_serverExecutor.lockExclusive() = kj::getCurrentThreadExecutor();

    auto exitPaf = kj::newPromiseAndFulfiller<void>();
    auto exitPromise = listenPromise.exclusiveJoin(kj::mv(exitPaf.promise));

    m_listenFulfiller = kj::mv(exitPaf.fulfiller);

    startUpPromise.set_value();
    exitPromise.wait(ioContext.waitScope);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  });

  startUpPromise.get_future().wait();

  return sThread;
}

ComponentServer::ComponentServer
(std::function<void(ComponentState const&)> f)
  : m_stateUpdateHandler(f)
{}

::kj::Promise<void>
ComponentServer::setComponentState
(SetComponentStateContext context)
{
  auto state = context.getParams().getState().cStr();

  LOG << "StateUpdate event received, new state : " << state; 

  m_stateUpdateHandler(state);
  
  return kj::READY_NOW;
}

} // namespace api
