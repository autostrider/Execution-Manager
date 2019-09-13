#include <adaptive_app.hpp>
#include <state.hpp>

#include <random>
#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

AdaptiveApp::AdaptiveApp(std::atomic<bool> &terminate, const std::string appName) 
  : m_sensorData(c_numberOfSamples),
    m_currentState{std::make_unique<Init>(*this)},
    m_terminateApp{terminate},
    m_appName{appName}
{
    m_currentState->enter();
}

void AdaptiveApp::transitToNextState()
{
    m_currentState->leave();
    m_currentState = m_currentState->handleTransition();
    m_currentState->enter();
}

double AdaptiveApp::mean()
{
    double sum = std::accumulate(m_sensorData.cbegin(), m_sensorData.cend(), 0.0);
    return sum / m_sensorData.size();
}

void AdaptiveApp::readSensorData()
{
    std::cout << "[ " << m_appName << " ]:\tRead data from sensors\n";

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
    std::cout << std::endl;
}

bool AdaptiveApp::isTerminating() const
{
    return m_terminateApp;
}

void AdaptiveApp::reportApplicationState(ApplicationState state)
{
    m_appClient.ReportApplicationState(state);
}

