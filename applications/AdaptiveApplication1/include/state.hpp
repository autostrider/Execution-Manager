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
protected:
    api::ApplicationStateClient::ApplicationState getApplicationState() const;
    AdaptiveApp& m_app;
    const api::ApplicationStateClient::ApplicationState m_applState;
    const std::string m_stateName;
};

class Init : public State
{
public:
    Init(AdaptiveApp& app);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
    void leave() const override;
};

class Run : public State
{
public:
    Run(AdaptiveApp& app);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

class Shutdown : public State
{
public:
    Shutdown(AdaptiveApp& app);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

#endif
