#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <vector>
#include <memory>
#include <atomic>

#include <application_state_client.h>

class State;
class AStateFactory;

class AdaptiveApp
{
public:
    AdaptiveApp(std::unique_ptr<AStateFactory> factory, std::unique_ptr<api::StateClient> client);
    virtual ~AdaptiveApp() = default;

    void transitToNextState(api::ApplicationStateClient::ApplicationState state);
    double mean();
    void readSensorData();
    void printSensorData() const;
    void reportApplicationState(api::ApplicationStateClient::ApplicationState state);

private:
    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<AStateFactory> m_factory;
    std::unique_ptr<State> m_currentState;
    std::unique_ptr<api::StateClient> m_appClient;
};
#endif
