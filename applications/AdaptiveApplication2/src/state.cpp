#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

State::State(AdaptiveApp &app, api::ApplicationStateClient::ApplicationState state, const std::string &stateName) :
    m_app{app}, m_applState{state}, m_stateName{stateName}
{
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
        return std::make_unique<Shutdown>(m_app);
    }
    return std::make_unique<Run>(m_app);
}

void Init::enter()
{
    std::cout << "[ AdaptiveApplication2 ]:\tReporting state "
              << m_stateName << "." << std::endl;
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
        return std::make_unique<Shutdown>(m_app);
    }
    return std::make_unique<Run>(m_app);
}

void Run::enter()
{
    m_app.readSensorData();
    std::cout << "[ AdaptiveApplication2 ]:\tSum: "
              << m_app.sum()
              << std::endl;
}

Shutdown::Shutdown(AdaptiveApp &app) : State (app, ApplicationState::K_SHUTTINGDOWN, "Shutdown")
{
}

std::unique_ptr<State> Shutdown::handleTransition()
{
    if (m_app.isTerminating())
    {
        std::cout << "[ AdaptiveApplication2 ]:\tAlready in Shutdown state!" << std::endl;
    }
    else
    {
        std::cout << "[ AdaptiveApplication2 ]:\tEntering Shutdown state." << std::endl;
    }
    return std::make_unique<Shutdown>(m_app);
}

void Shutdown::enter()
{
    std::cout << "[ AdaptiveApplication2 ]:\tReporting state "
              << m_stateName << "." << std::endl;

    std::cout << "[ AdaptiveApplication2 ]:\tIn Shutdown state." << std::endl
              << "[ AdaptiveApplication2 ]:\tIs dead..." << std::endl;
    m_app.reportApplicationState(getApplicationState());

    ::exit(EXIT_SUCCESS);
}
