#include <i_adaptive_app.hpp>
#include <i_state.hpp>
#include <i_state_factory.hpp>

namespace api
{
using ApplicationState = api::ApplicationStateClient::ApplicationState;
    
IAdaptiveApp::IAdaptiveApp(std::unique_ptr<IStateFactory> factory,
                std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(client)}
{
}
void IAdaptiveApp::transitToNextState(IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState = nextState(*this);
    m_currentState->enter();
    m_currentState->leave();
}

void IAdaptiveApp::reportApplicationState(ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}

} // namespace api