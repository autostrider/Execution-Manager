#include "i_component_client_wrapper.hpp"

namespace component_client
{

ComponentClientWrapper::ComponentClientWrapper(const std::string& component, 
                                               StateUpdateMode updateMode) noexcept
  : m_client{component, updateMode}
{}

ComponentClientReturnType ComponentClientWrapper::GetComponentState(ComponentState& state) noexcept
{
  return m_client.GetComponentState(state);
}

void ComponentClientWrapper::ConfirmComponentState(ComponentState state,
                                                   ComponentClientReturnType status) noexcept
{
  m_client.ConfirmComponentState(state, status);
}

ComponentState ComponentClientWrapper::setStateUpdateHandler() noexcept
{
    return m_client.setStateUpdateHandler();
}

} // namespace component_client