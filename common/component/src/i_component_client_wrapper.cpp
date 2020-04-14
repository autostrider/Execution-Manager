#include "i_component_client_wrapper.hpp"

namespace component_client
{

ComponentClientWrapper::ComponentClientWrapper(const std::string& component, 
                                               StateUpdateMode updateMode) noexcept
  : m_client{component, updateMode}
{}

void ComponentClientWrapper::setClient(std::unique_ptr<IClient> client)
{
  m_client.setClient(std::move(client));
}

ComponentClientReturnType ComponentClientWrapper::GetComponentState(ComponentState& state) noexcept
{
  return m_client.GetComponentState(state);
}

void ComponentClientWrapper::ConfirmComponentState(ComponentState state,
                                                   ComponentClientReturnType status) noexcept
{
  m_client.ConfirmComponentState(state, status);
}

ComponentClientReturnType ComponentClientWrapper::setStateUpdateHandler(std::function<void(ComponentState const&)> f) noexcept
{
    return m_client.setStateUpdateHandler(f);
}

void ComponentClientWrapper::checkIfAnyEventsAvailable()
{
  m_client.checkIfAnyEventsAvailable();
}

} // namespace component_client