#include <state.hpp>

#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

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
    std::cout << "Enter init state\n"
              << "App is created\n";
    app.reportApplicationState(this->getStateName());
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
    std::cout << "Enter run state\n";

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
    std::cout << "Enter terminate state\n" <<
            "Shut down app\n";
    app.reportApplicationState(this->getStateName());
    ::exit(EXIT_SUCCESS);
}

ApplicationState Terminate::getStateName() const
{
    return ApplicationState::K_SHUTTINGDOWN;
}
