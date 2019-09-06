#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

State::State(AdaptiveApp &app, api::ApplicationStateClient::ApplicationState state, std::string stateName) :
    m_app{app}, m_applState{state}, m_stateName{std::move(stateName)}
{
}

void State::leave() const
{

}

Init::Init(AdaptiveApp &app) : State (app, ApplicationState::K_INITIALIZING, "Initializing")
{
    std::cout << "Enter " << m_stateName << " state\n";
}

std::unique_ptr<State> Init::handleTransition()
{
    if (m_app.isTerminating())
    {
        return std::make_unique<Terminate>(m_app);
    }
    return std::make_unique<Run>(m_app);
}

void Init::enter()
{
    m_app.reportApplicationState(getApplicationState());
}

api::ApplicationStateClient::ApplicationState Init::getApplicationState() const
{
    return ApplicationState::K_INITIALIZING;
}

void Init::leave() const
{
    m_app.reportApplicationState(ApplicationState::K_RUNNING);
}

Run::Run(AdaptiveApp &app) : State (app, ApplicationState::K_RUNNING, "Running")
{
    std::cout << "Enter " << m_stateName << " state\n";
}

std::unique_ptr<State> Run::handleTransition()
{
    if (m_app.isTerminating())
    {
        return std::make_unique<Terminate>(m_app);
    }
    return std::make_unique<Run>(m_app);
}

void Run::enter()
{
    m_app.readSensorData();
    std::cout << "mean: " << m_app.mean() << "\n";
}

ApplicationState Run::getApplicationState() const
{
    return ApplicationState::K_RUNNING;
}

Terminate::Terminate(AdaptiveApp &app) : State (app, ApplicationState::K_SHUTTINGDOWN, "Terminating")
{
    std::cout << "Enter " << m_stateName << " state\n";
}

std::unique_ptr<State> Terminate::handleTransition()
{
    if (m_app.isTerminating())
    {
        std::cout << "Already in terminate state!";
    }
    else
    {
        std::cout << "terminating state. App is dead(\n";
    }
    return std::make_unique<Terminate>(m_app);
}

void Terminate::enter()
{
    std::cout << "App is dead...\n";
    m_app.reportApplicationState(getApplicationState());
    ::exit(EXIT_SUCCESS);
}

ApplicationState Terminate::getApplicationState() const
{
    return ApplicationState::K_SHUTTINGDOWN;
}
