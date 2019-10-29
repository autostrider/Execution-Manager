#include "i_component_client_wrapper.hpp"
#include <constants.hpp>

namespace api
{

ComponentClientWrapper::ComponentClientWrapper() noexcept
  : m_client{IPC_PROTOCOL + MSM_SOCKET_NAME, StateUpdateMode::kPoll}
{}

ComponentClientReturnType
ComponentClientWrapper::SetStateUpdateHandler
(std::function<void(ComponentState const&)> f) noexcept
{
  return m_client.SetStateUpdateHandler(f);
}

ComponentClientReturnType
ComponentClientWrapper::GetComponentState
(ComponentState& state) noexcept
{
  return m_client.GetComponentState(state);
}

void
ComponentClientWrapper::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  m_client.ConfirmComponentState(state, status);
}

} // namespace api