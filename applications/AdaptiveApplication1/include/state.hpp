#ifndef __STATE__
#define __STATE__

#include <adaptive_app.hpp>
#include <application_state_client.h>

class State
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, const std::string& stateName);
    virtual ~State() = default;

    virtual std::unique_ptr<State> handleTransition() = 0;
    virtual void enter() = 0;
    virtual void leave() const;
    api::ApplicationStateClient::ApplicationState getApplicationState() const;
protected:
    AdaptiveApp& m_app;
    const api::ApplicationStateClient::ApplicationState m_applState;
    const std::string m_stateName;
};

class Init : public State
{
public:
    Init(AdaptiveApp& app);
    virtual ~Init() = default;
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
    void leave() const override;
};

class Run : public State
{
public:
    Run(AdaptiveApp& app);
    virtual ~Run() = default;
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

class Terminate : public State
{
public:
    Terminate(AdaptiveApp& app);
    virtual ~Terminate() = default;
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

#endif
