#include "AppStateMachine.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>



void App::printVector() const
{
    for (const auto& item : _rawData) {
        std::cout << item << " | ";
    }
    std::cout << "\n";
}

bool App::isTerminating() const
{
    return terminateApp;
}

void App::terminate()
{
    terminateApp = true;
}

void App::readSensorData()
{
    std::cout << "Read data from sensors\n";

    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < numberOfSamples; ++k) {
        _rawData[k] = d(gen);
    }
}

double App::mean()
{
    double sum = std::accumulate(_rawData.cbegin(), _rawData.cend(), 0.0);
    sum /= _rawData.size();
    return  sum;
}

App::App() : _rawData(numberOfSamples), m_currentState{std::make_unique<Init>()}, terminateApp{false}
{
    _rawData.reserve(numberOfSamples);
    m_currentState->enter(*this);
}

void App::transitToNextState()
{
    auto newState = m_currentState->handleTransition(*this);
    m_currentState = std::move(newState);
    m_currentState->enter(*this);
}

App &App::getInstance()
{
   static App instance;
   return  instance;
}

std::unique_ptr<State> Init::handleTransition(App &app)
{
    if (app.isTerminating())
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Init::enter(App &app)
{
    std::cout << "Enter init state\n"
              << "App is created\n";
}

std::unique_ptr<State> Run::handleTransition(App &app)
{
    if (app.isTerminating())
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Run::enter(App &app)
{
    std::cout << "Enter run state\n";
    app.readSensorData();

    std::cout << "mean: " << app.mean() << "\n";
}

std::unique_ptr<State> Terminate::handleTransition(App &app)
{
    if (app.isTerminating())
    {
        std::cout << "Already in terminate state!";
    }
    else
    {
        std::cout << "terminating state. App is dead(\n";
    }
    return nullptr;
}

void Terminate::enter(App &app)
{
    std::cout << "Enter terminate state\n" <<
            "Shut down app\n";
    ::exit(EXIT_SUCCESS);
}
