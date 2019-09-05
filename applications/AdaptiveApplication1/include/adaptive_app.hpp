#ifndef __ADAPTIVE_APP__
#define __ADAPTIVE_APP__

#include <vector>
#include <memory>
#include <atomic>

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

private:
    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<State> m_currentState;
    std::atomic<bool>& m_terminateApp;
};
#endif
