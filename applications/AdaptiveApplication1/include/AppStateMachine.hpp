#ifndef __APP_STATE_MACHINE__
#define __APP_STATE_MACHINE__

#include <iostream>
#include <vector>
#include <memory>
#include <atomic>

class State;

class App
{
public:
    App(std::atomic<bool>& terminate);

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

class State
{
public:
    virtual ~State() = default;
    virtual std::unique_ptr<State> handleTransition(App& app) = 0;
    virtual void enter(App& app) = 0;
};

class Init : public State
{
public:
    std::unique_ptr<State> handleTransition(App& app) override;
    void enter(App& app) override;
};

class Run : public State
{
public:
    std::unique_ptr<State> handleTransition(App& app) override;
    void enter(App& app) override;
};

class Terminate : public State
{
    std::unique_ptr<State> handleTransition(App& app) override;
    void enter(App& app) override;
};

#endif
