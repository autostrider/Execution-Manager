#include "state.hpp"
#include <constants.hpp>
#include <logger.hpp>

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

void State::performAction()
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

Run::Run(AdaptiveApp &app) : State (app, ApplicationState::K_RUNNING, AA_STATE_RUNNING)
{
}

void Run::enter()
{
    m_app.reportApplicationState(getApplicationState());
}

void Run::performAction()
{
    LOG << "Mean: " << m_app.mean() << ".";
}

void Suspend::enter()
{
}

void Suspend::leave() const
{
}

void Suspend::performAction()
{

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

std::unique_ptr<api::IState> StateFactory::createSuspend(api::IAdaptiveApp &app) const
{
    return std::make_unique<Suspend>();
}
