#include <adaptive_app.hpp>
#include <state.hpp>
#include <logger.hpp>

#include <random>
#include <iostream>

AdaptiveApp::AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                         std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                         std::unique_ptr<api::IComponentClientWrapper> compClient) :
    m_factory{std::move(factory)},
    m_currentState{nullptr},
    m_appClient{std::move(appClient)},
    m_componentClient{std::move(compClient)}
{

}

void AdaptiveApp::init()
{
   m_currentState = m_factory->createInit(*this);
   m_currentState->enter();
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
                std::bind(&api::IStateFactory::createShutDown, m_factory.get(), std::placeholders::_1)
                );
}

double AdaptiveApp::mean()
{
    auto data = readSensorData();
    double sum = std::accumulate(data.cbegin(), data.cend(), 0.0);
    return sum / data.size();
}

std::vector<double> AdaptiveApp::readSensorData()
{
    LOG << "Read data from sensors.";

    const size_t c_numberOfSamples = 50;
    std::vector<double> data(c_numberOfSamples);
    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < c_numberOfSamples; ++k) {
        data[k] = d(gen);
    }
    return data;
}

void AdaptiveApp::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}
void AdaptiveApp::reportApplicationState(api::ApplicationStateClient::ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}

api::ComponentClientReturnType AdaptiveApp::getComponentState(api::ComponentState &state) noexcept
{
    return m_componentClient->GetComponentState(state);
}

void AdaptiveApp::confirmComponentState(api::ComponentState state, api::ComponentClientReturnType status) noexcept
{
    m_componentClient->ConfirmComponentState(state, status);
}
