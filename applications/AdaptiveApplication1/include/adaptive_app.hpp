#ifndef __ADAPTIVE_APP__
#define __ADAPTIVE_APP__

#include <vector>
#include <memory>
#include <atomic>
#include <iostream>

#include <application_state_client.h>

class State;

class AdaptiveApp
{
public:
    AdaptiveApp(std::atomic<bool>& terminate);
    virtual ~AdaptiveApp() = default;

    void init(std::unique_ptr<State> initialState, std::unique_ptr<api::ApplicationStateClient> client);
    void transitToNextState();
    virtual double mean();
    virtual void readSensorData();
    void printSensorData() const;
    virtual bool isTerminating() const;
    virtual void reportApplicationState(api::ApplicationStateClient::ApplicationState state);

private:
    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<State> m_currentState;
    std::atomic<bool>& m_terminateApp;
    std::unique_ptr<api::ApplicationStateClient> m_appClient;
};
#endif
