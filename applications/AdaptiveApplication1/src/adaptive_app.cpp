#include <adaptive_app.hpp>
#include <state.hpp>
#include <logger.hpp>

#include <i_state_factory.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <application_state_client.h>
#include <i_component_client_wrapper.hpp>
#include "i_mean_calculator.hpp"

AdaptiveApp::AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                         std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                         std::unique_ptr<api::IComponentClientWrapper> compClient, std::unique_ptr<IMeanCalculator> meanCalculator) :
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(appClient)},
    m_componentClient{std::move(compClient)},
    m_meanCalculator{std::move(meanCalculator)}
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

void AdaptiveApp::suspend()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createSuspend, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::performAction()
{
    api::ComponentState state;
    auto result = m_componentClient->GetComponentState(state);
    auto confirm = api::ComponentClientReturnType::K_GENERAL_ERROR;

    LOG << "Current ComponentState: " << m_componentState;

    if (api::ComponentClientReturnType::K_SUCCESS == result)
    {
        if (isValid(state))
        {
            if (shouldResume(state))
            {
                run();
                m_componentState = api::ComponentStateKOn;
                LOG << "ComponentState updated to: " << m_componentState;
                confirm = api::ComponentClientReturnType::K_SUCCESS;
            }
            else if (shouldSuspend(state))
            {
                suspend();
                m_componentState = api::ComponentStateKOff;
                LOG << "ComponentState updated to: " << m_componentState;
                confirm = api::ComponentClientReturnType::K_SUCCESS;
            }
            else
            {
                confirm = api::ComponentClientReturnType::K_UNCHANGED;
            }
        }
        else
        {
            confirm = api::ComponentClientReturnType::K_INVALID;
        }
    }
    else
    {
        confirm = result;
    }
    m_componentClient->ConfirmComponentState(state, confirm);
    m_currentState->performAction();
}

double AdaptiveApp::mean()
{
    return m_meanCalculator->mean();
}

void AdaptiveApp::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}

bool AdaptiveApp::shouldChange(const api::ComponentState &state) const
{
    return m_componentState != state;
}

bool AdaptiveApp::isValid(const api::ComponentState &state) const
{
    return api::ComponentStateKOn == state || api::ComponentStateKOff == state;
}

bool AdaptiveApp::shouldResume(const api::ComponentState& state) const
{
    return (m_componentState != state && api::ComponentStateKOn == state);
}

bool AdaptiveApp::shouldSuspend(const api::ComponentState& state) const
{
    return (m_componentState != state && api::ComponentStateKOff == state);
}

void AdaptiveApp::reportApplicationState(api::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
