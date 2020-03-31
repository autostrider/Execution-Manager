#include "component_client.h"
#include <constants.hpp>
#include <logger.hpp>
#include <thread>

using namespace constants;

namespace component_client
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

  request.setComponent(m_componentName);
  request.setState(state);
  request.setStatus(status);

  request.send().wait(m_rpcClient.getWaitScope());
 }

} // namespace component_client
