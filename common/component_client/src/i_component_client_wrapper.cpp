#include "i_component_client_wrapper.hpp"
#include <constants.hpp>

namespace component_client
{

ComponentClientWrapper::ComponentClientWrapper(const std::string& component, 
                                               StateUpdateMode updateMode) noexcept
  : m_client{component, updateMode}
{}

ComponentClientWrapper::~ComponentClientWrapper() noexcept
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

void ComponentClientWrapper::CheckIfAnyEventsAvailable() noexcept
{
  m_client.checkIfAnyEventsAvailable();
}

} // namespace component_client