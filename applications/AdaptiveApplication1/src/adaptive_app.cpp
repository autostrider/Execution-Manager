#include <adaptive_app.hpp>
#include <state.hpp>

#include <random>
#include <iostream>

AdaptiveApp::AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
    api::IAdaptiveApp(std::move(factory), std::move(client)),
    m_sensorData(c_numberOfSamples)
{
    transitToNextState(
                std::bind(&api::IStateFactory::createInit, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::run()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createRun, m_factory.get(), std::placeholders::_1)
                );
}

void AdaptiveApp::terminate()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createTerminate, m_factory.get(), std::placeholders::_1)
                );
}

double AdaptiveApp::mean()
{
    double sum = std::accumulate(m_sensorData.cbegin(), m_sensorData.cend(), 0.0);
    return sum / m_sensorData.size();
}

void AdaptiveApp::readSensorData()
{
    std::cout << "Read data from sensors" << std::endl;

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
