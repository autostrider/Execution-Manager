#include "AppStateMachine.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>



void App::printVector(const std::vector<double>& vec) const
{
    for (const auto& item : vec) {
        std::cout << item << " | ";
    }
    std::cout << "\n";
}

void App::readSensorData()
{
    std::cout << "Read data from sensors\n";
    const size_t numberOfSamples = 100;
    std::vector<double> data(numberOfSamples);
    data.reserve(numberOfSamples);

    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < numberOfSamples; ++k) {
        data[k] = d(gen);
    }
    _rawData = data;
}

double App::mean()
{
    double sum = std::accumulate(_rawData.cbegin(), _rawData.cend(), 0);
    std::cout << "";
    sum /= _rawData.size();
    return  sum;
}

App::App()
{
    m_currentState = std::make_unique<Init>();
    m_currentState->enter(*this);
}

void App::transitToNextState(int state)
{
    auto newState = m_currentState->handleTransition(*this, state);
    if (newState != nullptr)
    {
        m_currentState = std::move(newState);
        m_currentState->enter(*this);
    }
}

App &App::getInstance()
{
   static App instance;
   return  instance;
}

std::unique_ptr<State> Init::handleTransition(App &app, int state)
{
    if (state == App::TerminateApp)
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Init::enter(App &app)
{
    std::cout << "Enter init state\n";
    app.readSensorData();
}

std::unique_ptr<State> Run::handleTransition(App &app, int state)
{
    if (state == App::TerminateApp)
    {
        return std::make_unique<Terminate>();
    }
    return std::make_unique<Run>();
}

void Run::enter(App &app)
{
    std::cout << "Enter run state\n";
    std::cout << "mean: " << app.mean() << "\n";
}

std::unique_ptr<State> Terminate::handleTransition(App &app, int state)
{
    if (state == App::TerminateApp)
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
