#include "component_client.h"
#include <constants.hpp>
#include <logger.hpp>

namespace api
{
ComponentClient::ComponentClient
(const std::string &s, StateUpdateMode mode) noexcept
: m_client(IPC_PROTOCOL + EM_SOCKET_NAME), componentName(s), updateMode(mode)
{
  auto cap = m_client.getMain<StateManagement>();

  auto request = cap.registerComponentRequest();
  request.setComponent(componentName);

  request.send().wait(m_client.getWaitScope());
}

ComponentClientReturnType
ComponentClient::SetStateUpdateHandler
(std::function<void(ComponentState const&)> f) noexcept
{
  updateHandler = f;
  return ComponentClientReturnType::kSuccess;
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  auto cap = m_client.getMain<StateManagement>();

  auto request = cap.getComponentStateRequest();
  request.setComponent(componentName);

  auto result = request.send().wait(m_client.getWaitScope());

  state = result.getState();

  return result.getResult();
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  auto cap = m_client.getMain<StateManagement>();

  auto request = cap.confirmComponentStateRequest();

  request.setComponent(componentName);
  request.setState(state);
  request.setStatus(status);

  request.send().wait(m_client.getWaitScope());
}

} // namespace api