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
    virtual ~App() = default;
    App(std::atomic<bool>& termRef);

    void transitToNextState();
    double mean();
    void readSensorData();
    void printVector() const;

    bool isTerminating() const;

private:
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    const size_t numberOfSamples = 50;
    std::vector<double> _rawData;
    std::unique_ptr<State> m_currentState;
    std::atomic<bool>& terminateApp;
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
