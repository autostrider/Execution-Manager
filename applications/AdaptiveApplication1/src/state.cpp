#include "state.hpp"
#include <constants.hpp>
#include <logger.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

State::State(AdaptiveApp& app, 
             ApplicationState state,
             std::string stateName) :
    m_app{app},
    m_applState{state},
    m_stateName{std::move(stateName)}
{
    LOG << "Enter " << m_stateName << " state.";
}

void State::leave() const
{
}

ApplicationState State::getApplicationState() const
{
    return m_applState;
}

Init::Init(AdaptiveApp &app) : State (app, ApplicationState::K_INITIALIZING, AA_STATE_INIT)
{
}

void Init::enter()
{
    m_app.reportApplicationState(getApplicationState());
}

void Init::leave() const
{
    m_app.reportApplicationState(ApplicationState::K_RUNNING);
}

Run::Run(AdaptiveApp &app) : State (app, ApplicationState::K_RUNNING, AA_STATE_RUNNING)
{
}

void Run::enter()
{
    api::ComponentState state;
    auto result = m_app.getComponentState(state);
    if (api::ComponentClientReturnType::kSuccess == result)
    {
        m_app.confirmComponentState(state, handleTransition(state));
    }
    else
    {
        m_app.confirmComponentState(state, result);
    }

}

api::ComponentClientReturnType Run::handleTransition(api::ComponentState state)
{
    api::ComponentClientReturnType confirm = api::ComponentClientReturnType::kInvalid;

    if (m_subState != state)
    {
        if (api::ComponentStateKOn == state)
        {
            m_app.readSensorData();
            LOG << "Mean: " << m_app.mean() << ".";
            m_subState = state;
            confirm = api::ComponentClientReturnType::kSuccess;
        }
        else if (api::ComponentStateKOff == state)
        {
            /*some important stuff related to suspend state
            * this transition to kOff state will refactored.
            */
            m_subState = state;
            confirm = api::ComponentClientReturnType::kSuccess;
        }
    }
    else
    {
        if (api::ComponentStateKOn == state)
        {
            m_app.readSensorData();
            LOG << "Mean: " << m_app.mean() << ".";
        }
        else if (api::ComponentStateKOff == state)
        {
            /*some important stuff related to suspend state
            * this transition to kOff state will refactored.
            */
        }
        confirm = api::ComponentClientReturnType::kUnchanged;
    }

    return confirm;
}

ShutDown::ShutDown(AdaptiveApp& app) : State (app, ApplicationState::K_SHUTTINGDOWN, AA_STATE_SHUTDOWN)
{
}

void ShutDown::enter()
{
    m_app.reportApplicationState(getApplicationState());
    LOG << "Killing app...";
}

std::unique_ptr<api::IState> StateFactory::createInit(api::IAdaptiveApp& app) const
{
    return std::make_unique<Init>(dynamic_cast<AdaptiveApp&>(app));
}

std::unique_ptr<api::IState> StateFactory::createRun(api::IAdaptiveApp& app) const
{
    return std::make_unique<Run>(dynamic_cast<AdaptiveApp&>(app));
}

std::unique_ptr<api::IState> StateFactory::createShutDown(api::IAdaptiveApp& app) const
{
    return std::make_unique<ShutDown>(dynamic_cast<AdaptiveApp&>(app));
}

