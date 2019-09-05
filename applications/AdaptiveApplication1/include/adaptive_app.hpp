#ifndef __ADAPTIVE_APP__
#define __ADAPTIVE_APP__

#include <vector>
#include <memory>
#include <atomic>

#include <application_state_client.h>

class State;

class AdaptiveApp
{
public:
    AdaptiveApp(std::atomic<bool>& terminate);

    void transitToNextState();
    double mean();
    void readSensorData();
    void printSensorData() const;
    bool isTerminating() const;
    void ReportApplicationState(api::ApplicationStateClient::ApplicationState state);

private:
    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<State> m_currentState;
    std::atomic<bool>& m_terminateApp;
    api::ApplicationStateClient m_appClient;
};
#endif
