#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <functional>
#include <vector>
#include <memory>
#include <atomic>

#include <application_state_client.h>

class IState;
class IStateFactory;

class AdaptiveApp
{
    using FactoryFunc = std::function<std::unique_ptr<IState>(AdaptiveApp&)>;
public:
    AdaptiveApp(std::unique_ptr<IStateFactory> factory,
                std::unique_ptr<api::IApplicationStateClientWrapper> client);
    virtual ~AdaptiveApp() = default;

    void run();
    void terminate();

    double mean();
    void readSensorData();
    void printSensorData() const;
    void reportApplicationState(api::ApplicationStateClient::ApplicationState state);

private:
    void transitToNextState(FactoryFunc nextState);

    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<IStateFactory> m_factory;
    std::unique_ptr<IState> m_currentState;
    std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;
};
#endif
