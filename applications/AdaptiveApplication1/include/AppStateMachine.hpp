#ifndef __APP_STATE_MACHINE__
#define __APP_STATE_MACHINE__

#include <iostream>
#include <vector>
#include <memory>

class State;

class App
{
public:
    App();
    virtual ~App(){}

    virtual void transitToNextState(int state = 0);
    double mean();
    void readSensorData();
    void printVector(const std::vector<double>& vec) const;

    const static int TerminateApp = -1;
private:
    std::vector<double> _rawData;
    std::unique_ptr<State> m_currentState;

};

class State
{
public:
    virtual ~State(){}
    virtual std::unique_ptr<State> handleTransition(App& app, int state) = 0;
    virtual void enter(App& app) = 0;
};

class Init : public State
{
public:
    std::unique_ptr<State> handleTransition(App& app, int state) override;
    void enter(App& app) override;
};

class Run : public State
{
public:
    std::unique_ptr<State> handleTransition(App& app, int state) override;
    void enter(App& app) override;
};

class Terminate : public State
{
    std::unique_ptr<State> handleTransition(App& app, int state) override;
    void enter(App& app) override;
};

#endif
