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

void AdaptiveApp::performAction()
{
    api::ComponentState state;
    auto result = m_componentClient->GetComponentState(state);
    if (api::ComponentClientReturnType::K_SUCCESS == result)
    {
        auto confirm = handleTransition(state);
        if (api::ComponentClientReturnType::K_SUCCESS == confirm)
        {
            //state changed from\to kOn\kOff
            if (api::ComponentStateKOn == state)
            {
                transitToNextState(
                            std::bind(&api::IStateFactory::createRun,
                                      m_factory.get(),
                                      std::placeholders::_1)
                            );
            }
            else if (api::ComponentStateKOff == state)
            {
                transitToNextState(
                            std::bind(&api::IStateFactory::createSuspend,
                                      m_factory.get(),
                                      std::placeholders::_1)
                            );
            }
        }
        m_componentClient->ConfirmComponentState(state, confirm);
    }
    else
    {
        m_componentClient->ConfirmComponentState(state, result);
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

api::ComponentClientReturnType AdaptiveApp::handleTransition(const api::ComponentState &state)
{
    api::ComponentClientReturnType confirm = api::ComponentClientReturnType::K_INVALID;

    LOG << "Current CompState: " << m_componentState;
    if (m_componentState != state)
    {
        if (api::ComponentStateKOn == state || api::ComponentStateKOff == state)
        {
            LOG << "Updating ComponentState to: " << state;
            m_componentState = state;
            confirm = api::ComponentClientReturnType::K_SUCCESS;
        }
        return confirm;
    }
    else
    {
        LOG << "ComponentState is unchanged.";
        confirm = api::ComponentClientReturnType::K_UNCHANGED;
    }

    return confirm;
}

void AdaptiveApp::reportApplicationState(api::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
