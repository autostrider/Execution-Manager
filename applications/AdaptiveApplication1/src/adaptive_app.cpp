#include <adaptive_app.hpp>
#include <state.hpp>

#include <random>
#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

AdaptiveApp::AdaptiveApp(std::atomic<bool> &terminate) : m_sensorData(c_numberOfSamples),
    m_currentState{nullptr},
    m_terminateApp{terminate},
    m_appClient{nullptr}
{
}

void AdaptiveApp::init(std::unique_ptr<State> initialState, std::unique_ptr<api::ApplicationStateClient> client)
{
    m_appClient = std::move(client);
    m_currentState = std::move(initialState);
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

void AdaptiveApp::reportApplicationState(ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
