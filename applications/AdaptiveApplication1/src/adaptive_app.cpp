#include <adaptive_app.hpp>
#include <state.hpp>

#include <random>
#include <iostream>

AdaptiveApp::AdaptiveApp(std::atomic<bool> &terminate) : m_sensorData(c_numberOfSamples), m_currentState{std::make_unique<Init>()}, m_terminateApp{terminate}
{
    m_sensorData.reserve(c_numberOfSamples);
    m_currentState->enter(*this);
}

void AdaptiveApp::transitToNextState()
{
    m_currentState = m_currentState->handleTransition(*this);
    m_currentState->enter(*this);
}

double AdaptiveApp::mean()
{
    double sum = std::accumulate(m_sensorData.cbegin(), m_sensorData.cend(), 0.0);
    return sum / m_sensorData.size();
}

void AdaptiveApp::readSensorData()
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

void AdaptiveApp::printSensorData() const
{
    for (const auto& item : m_sensorData) {
        std::cout << item << " | ";
    }
    std::cout << "\n";
}

bool AdaptiveApp::isTerminating() const
{
    return m_terminateApp;
}

