#include "AppStateMachine.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>



void App::printSensorData() const
{
    for (const auto& item : m_sensorData) {
        std::cout << item << " | ";
    }
    std::cout << "\n";
}

bool App::isTerminating() const
{
    return m_terminateApp.load();
}

void App::readSensorData()
{
    std::cout << "Read data from sensors\n";

    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < c_numberOfSamples; ++k) {
        m_sensorData[k] = d(gen);
    }
}

double App::mean()
{
    double sum = std::accumulate(m_sensorData.cbegin(), m_sensorData.cend(), 0.0);
    return sum / m_sensorData.size();
}

App::App(std::atomic<bool> &terminate) : m_sensorData(c_numberOfSamples), m_currentState{std::make_unique<Init>()}, m_terminateApp{terminate}
{
    m_sensorData.reserve(c_numberOfSamples);
    m_currentState->enter(*this);
}

void App::transitToNextState()
{
    m_currentState = m_currentState->handleTransition(*this);
    m_currentState->enter(*this);
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
    return std::make_unique<Terminate>();
}

void Terminate::enter(App &app)
{
    std::cout << "Enter terminate state\n" <<
            "Shut down app\n";
    ::exit(EXIT_SUCCESS);
}
