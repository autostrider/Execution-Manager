#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

State::State(AdaptiveApp &app, api::ApplicationStateClient::ApplicationState state, const std::string &stateName) :
    m_app{app}, m_applState{state}, m_stateName{stateName}
{
    std::cout << "Enter " << m_stateName << " state\n";
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
    std::cout << "mean: " << m_app.mean() << "\n";
}

Terminate::Terminate(AdaptiveApp &app) : State (app, ApplicationState::K_SHUTTINGDOWN, "Terminating")
{
}

void Terminate::enter()
{
    m_app.reportApplicationState(getApplicationState());
    std::cout << "Killing app...\n";
}

std::unique_ptr<IState> StateFactory::createInit(AdaptiveApp &app)
{
    return std::make_unique<Init>(app);
}

std::unique_ptr<IState> StateFactory::createRun(AdaptiveApp &app)
{
    return std::make_unique<Run>(app);
}

std::unique_ptr<IState> StateFactory::createTerminate(AdaptiveApp &app)
{
    return std::make_unique<Terminate>(app);
}
