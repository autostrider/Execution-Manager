#include <adaptive_app.hpp>
#include <state.hpp>
#include <logger.hpp>

#include <application_state_client.h>
#include <i_state_factory.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <i_component_client_wrapper.hpp>
#include "i_mean_calculator.hpp"

using namespace component_client;

AdaptiveApp::AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                         std::unique_ptr<application_state::IApplicationStateClientWrapper> appClient,
                         std::unique_ptr<IComponentClientWrapper> compClient,
                         std::unique_ptr<api::IMeanCalculator> meanCalculator,
                         bool eventModeEnabled) :
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(appClient)},
    m_componentClient{std::move(compClient)},
    m_meanCalculator{std::move(meanCalculator)},
    m_eventModeEnabled{eventModeEnabled}
{
    if (m_eventModeEnabled)
    {
        m_componentClient->SetStateUpdateHandler(std::bind(&AdaptiveApp::stateUpdateHandler,
                                                           this, 
                                                           std::placeholders::_1));
    }
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

void AdaptiveApp::stateUpdateHandler(ComponentState const& state)
{
    m_componentClient->ConfirmComponentState(state, setComponentState(state));
}

void AdaptiveApp::pollComponentState()
{
    ComponentState state;
    auto result = m_componentClient->GetComponentState(state);
    
    if (ComponentClientReturnType::kSuccess == result)
    {
        result = setComponentState(state);
    }

    m_componentClient->ConfirmComponentState(state, result);
}

ComponentClientReturnType AdaptiveApp::setComponentState(ComponentState const& state)
{
    auto setStateResult = ComponentClientReturnType::kSuccess;

    if (isValid(state))
    {
        if (shouldResume(state))
        {
            run();
        }
        else if (shouldSuspend(state))
        {
            suspend();
        }
        else
        {
            setStateResult = ComponentClientReturnType::kUnchanged;
        }
    }
    else
    {
        setStateResult = ComponentClientReturnType::kInvalid;
    }
  
    return setStateResult;
}

void AdaptiveApp::performAction()
{
    if (m_eventModeEnabled)
    {
        m_componentClient->CheckIfAnyEventsAvailable();
    }
    else
    {
        pollComponentState();
    }
    

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

bool AdaptiveApp::isValid(const ComponentState &state) const
{
    return COMPONENT_STATE_ON == state || COMPONENT_STATE_OFF == state;
}

bool AdaptiveApp::shouldResume(const ComponentState& state) const
{
    return (m_componentState != state && COMPONENT_STATE_ON == state);
}

bool AdaptiveApp::shouldSuspend(const ComponentState& state) const
{
    return (m_componentState != state && COMPONENT_STATE_OFF == state);
}

void AdaptiveApp::reportApplicationState(application_state::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
