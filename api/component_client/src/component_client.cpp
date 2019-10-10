#include "component_client.h"

namespace api
{
ComponentClient::ComponentClient
(const std::string &s, StateUpdateMode mode) noexcept
{}

ComponentClientReturnType
ComponentClient::SetStateUpdateHandler
(std::function<void(ComponentState const&)> f) noexcept
{
  return ComponentClientReturnType::K_SUCCESS;
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  return ComponentClientReturnType::K_SUCCESS;
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
}

} // namespace api