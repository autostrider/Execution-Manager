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
    m_componentState = COMPONENT_STATE_ON;
    LOG << "ComponentState updated to: " << m_componentState;
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
    m_componentState = COMPONENT_STATE_OFF;
    LOG << "ComponentState updated to: " << m_componentState;
}

void AdaptiveApp::performAction()
{
    api::ComponentState state;
    auto result = m_componentClient->GetComponentState(state);
    auto confirm = api::ComponentClientReturnType::kGeneralError;

    LOG << "Current ComponentState: " << m_componentState;

    if (api::ComponentClientReturnType::kSuccess == result)
    {
        if (isValid(state))
        {
            if (shouldResume(state))
            {
                run();
                confirm = api::ComponentClientReturnType::kSuccess;
            }
            else if (shouldSuspend(state))
            {
                suspend();
                confirm = api::ComponentClientReturnType::kSuccess;
            }
            else
            {
                confirm = api::ComponentClientReturnType::kUnchanged;
            }
        }
        else
        {
            confirm = api::ComponentClientReturnType::kInvalid;
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
    return COMPONENT_STATE_ON == state || COMPONENT_STATE_OFF == state;
}

bool AdaptiveApp::shouldResume(const api::ComponentState& state) const
{
    return (m_componentState != state && COMPONENT_STATE_ON == state);
}

bool AdaptiveApp::shouldSuspend(const api::ComponentState& state) const
{
    return (m_componentState != state && COMPONENT_STATE_OFF == state);
}

void AdaptiveApp::reportApplicationState(api::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
