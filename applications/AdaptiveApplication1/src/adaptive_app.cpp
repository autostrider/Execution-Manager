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
                         std::unique_ptr<api::IComponentClientWrapper> compClient,
                         std::unique_ptr<IMeanCalculator> meanCalculator,
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
    m_componentState = api::ComponentStateKOn;
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
    m_componentState = api::ComponentStateKOff;
    LOG << "ComponentState updated to: " << m_componentState;
}

void AdaptiveApp::stateUpdateHandler(api::ComponentState const& state)
{

    LOG << "........................stateUpdateHandler called...................." ;
    m_componentClient->ConfirmComponentState(state, setComponentState(state));
    // setComponentState(state);
}

void AdaptiveApp::pollComponentState()
{
        LOG << "........................pollComponentState...................." ;

    api::ComponentState state;
    auto result = m_componentClient->GetComponentState(state);
    
    if (api::ComponentClientReturnType::K_SUCCESS == result)
    {
        result = setComponentState(state);
    }

    m_componentClient->ConfirmComponentState(state, result);
}

api::ComponentClientReturnType AdaptiveApp::setComponentState(api::ComponentState const& state)
{
    auto setStateResult = api::ComponentClientReturnType::K_SUCCESS;

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
            setStateResult = api::ComponentClientReturnType::K_UNCHANGED;
        }
    }
    else
    {
        setStateResult = api::ComponentClientReturnType::K_INVALID;
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
