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
    m_app.readSensorData();
    LOG << "Mean: " << m_app.mean() << ".";
}

ShutDown::ShutDown(AdaptiveApp& app) : State (app, ApplicationState::K_SHUTTINGDOWN, AA_STATE_SHUTDOWN)
{
}

void ShutDown::enter()
{
    m_app.reportApplicationState(getApplicationState());
    LOG << "Killing app...";
}

Suspend::Suspend(AdaptiveApp &app) : State(app, ApplicationState::K_SUSPEND, AA_STATE_SUSPEND)
{

}

void Suspend::enter()
{
    m_app.reportApplicationState(getApplicationState());
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

std::unique_ptr<api::IState> StateFactory::createSuspend(api::IAdaptiveApp &app) const
{
    return std::make_unique<Suspend>(dynamic_cast<AdaptiveApp&>(app));
}

