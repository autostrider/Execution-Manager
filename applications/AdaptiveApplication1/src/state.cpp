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

void Init::leave() const
{
    m_app.reportApplicationState(ApplicationState::K_RUNNING);
}

Run::Run(AdaptiveApp &app) : State (app, ApplicationState::K_RUNNING, "Running")
{
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

Terminate::Terminate(AdaptiveApp &app) : State (app, ApplicationState::K_SHUTTINGDOWN, "Terminating")
{
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
    m_app.reportApplicationState(getApplicationState());
    throw std::runtime_error("killing app...\n");
}
