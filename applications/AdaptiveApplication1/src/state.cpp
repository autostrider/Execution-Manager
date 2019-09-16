#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

State::State(AdaptiveApp &app, api::ApplicationStateClient::ApplicationState state, const std::string &stateName) :
    m_app{app}, m_applState{state}, m_stateName{stateName}
{
    std::cout << "Enter " << m_stateName << " state" << std::endl;
}

void State::leave() const
{

}

api::ApplicationStateClient::ApplicationState State::getApplicationState() const
{
    return m_applState;
}

Init::Init(AdaptiveApp &app) : State (app, ApplicationState::K_INITIALIZING, "Initializing")
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

Run::Run(AdaptiveApp &app) : State (app, ApplicationState::K_RUNNING, "Running")
{
}

void Run::enter()
{
    m_app.readSensorData();
    std::cout << "mean: " << m_app.mean() << std::endl;
}

ShutDown::ShutDown(AdaptiveApp &app) : State (app, ApplicationState::K_SHUTTINGDOWN, "Shutdown")
{
}

void ShutDown::enter()
{
    m_app.reportApplicationState(getApplicationState());
    std::cout << "Killing app..." << std::endl;
}

std::unique_ptr<api::IState> StateFactory::createInit(api::IAdaptiveApp &app)
{
    return std::make_unique<Init>(dynamic_cast<AdaptiveApp&>(app));
}

std::unique_ptr<api::IState> StateFactory::createRun(api::IAdaptiveApp &app)
{
    return std::make_unique<Run>(dynamic_cast<AdaptiveApp&>(app));
}

std::unique_ptr<api::IState> StateFactory::createShutDown(api::IAdaptiveApp &app)
{
    return std::make_unique<ShutDown>(dynamic_cast<AdaptiveApp&>(app));
}
