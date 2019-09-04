#include <state.hpp>

#include <iostream>

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
    ::exit(EXIT_SUCCESS);
}
