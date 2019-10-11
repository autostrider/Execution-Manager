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

    if (
            api::ComponentClientReturnType::K_SUCCESS == result &&
            api::ComponentStatesToString.at(api::ComponentStates::kOn) == state
            )
    {
        m_app.readSensorData();
        LOG << "Mean: " << m_app.mean() << ".";
        m_app.confirmComponentState(state, api::ComponentClientReturnType::K_SUCCESS);
        LOG << "State kOn is set.";
    }
    else if (
             api::ComponentClientReturnType::K_SUCCESS == result &&
             api::ComponentStatesToString.at(api::ComponentStates::kOff) == state
             )
    {
        m_app.reportApplicationState(api::ApplicationStateClient::ApplicationState::K_SUSPEND);
        /*some important stuff related to suspend state*/
        m_app.confirmComponentState(state, api::ComponentClientReturnType::K_SUCCESS);
        LOG << "State kOff is set.";
    }
    else
    {
        m_app.confirmComponentState(state, api::ComponentClientReturnType::K_INVALID);
        LOG << "Invalid component state received.";
    }
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

