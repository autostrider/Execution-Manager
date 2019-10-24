#include <adaptive_app.hpp>
#include <state.hpp>
#include <logger.hpp>

#include <random>
#include <iostream>

AdaptiveApp::AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                         std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                         std::unique_ptr<api::IComponentClientWrapper> compClient, std::unique_ptr<IAdaptiveAppBase> baseApp) :
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(appClient)},
    m_componentClient{std::move(compClient)},
    m_baseApp{std::move(baseApp)}
{

}

void AdaptiveApp::init()
{
    m_currentState = m_factory->createInit(*this);
    m_currentState->enter();
}

void AdaptiveApp::run()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createRun, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::terminate()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createShutDown, m_factory.get(), std::placeholders::_1)
                );
}

double AdaptiveApp::mean()
{
    return m_baseApp->mean();
}

void AdaptiveApp::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}
void AdaptiveApp::reportApplicationState(api::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}

api::ComponentClientReturnType AdaptiveApp::getComponentState(api::ComponentState &state) noexcept
{
    return m_componentClient->GetComponentState(state);
}

void AdaptiveApp::confirmComponentState(api::ComponentState state, api::ComponentClientReturnType status) noexcept
{
    m_componentClient->ConfirmComponentState(state, status);
}
