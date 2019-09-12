#include <adaptive_app.hpp>
#include <state.hpp>

#include <random>
#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;

AdaptiveApp::AdaptiveApp(std::unique_ptr<IStateFactory> factory,
                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    m_sensorData(c_numberOfSamples),
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(client)}
{
    transitToNextState(
                std::bind(&IStateFactory::makeInit, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::run()
{
    transitToNextState(
                std::bind(&IStateFactory::makeRun, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::terminate()
{
    transitToNextState(
                std::bind(&IStateFactory::makeTerminate, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::transitToNextState(FactoryFunc nextState)
{
    m_currentState = nextState(*this);
    m_currentState->enter();
    m_currentState->leave();
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

void AdaptiveApp::reportApplicationState(ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}
