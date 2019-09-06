#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

const std::unordered_map<ApplicationState, std::string>State::c_stateToString =
{
    {ApplicationState::K_INITIALIZING, "Initializing"},
    {ApplicationState::K_RUNNING, "Running"},
    {ApplicationState::K_SHUTTINGDOWN, "Terminating"},
};

void State::enter(AdaptiveApp &app)
{
    std::cout << "Enter " << c_stateToString.at(getStateName())
              << " state\n";
}

void State::leave(AdaptiveApp &app) const
{

}

std::unique_ptr<State> Init::handleTransition(AdaptiveApp &app)
{
    if (app.isTerminating())
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Init::enter(AdaptiveApp &app)
{
    State::enter(app);
    app.reportApplicationState(getStateName());
}

ApplicationState Init::getStateName() const
{
    return ApplicationState::K_INITIALIZING;
}

void Init::leave(AdaptiveApp &app) const
{
    app.reportApplicationState(ApplicationState::K_RUNNING);
}

std::unique_ptr<State> Run::handleTransition(AdaptiveApp &app)
{
    if (app.isTerminating())
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Run::enter(AdaptiveApp &app)
{
    State::enter(app);
    app.readSensorData();
    std::cout << "mean: " << app.mean() << "\n";
}

ApplicationState Run::getStateName() const
{
    return ApplicationState::K_RUNNING;
}

std::unique_ptr<State> Terminate::handleTransition(AdaptiveApp &app)
{
    if (app.isTerminating())
    {
        std::cout << "Already in terminate state!";
    }
    else
    {
        std::cout << "terminating state. App is dead(\n";
    }
    return std::make_unique<Terminate>();
}

void Terminate::enter(AdaptiveApp &app)
{
    State::enter(app);
    std::cout << "App is dead...\n";
    app.reportApplicationState(getStateName());
    ::exit(EXIT_SUCCESS);
}

ApplicationState Terminate::getStateName() const
{
    return ApplicationState::K_SHUTTINGDOWN;
}
