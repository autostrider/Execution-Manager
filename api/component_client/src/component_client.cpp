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
  return ComponentClientReturnType::kSuccess;
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  return ComponentClientReturnType::kSuccess;
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
}

} // namespace api