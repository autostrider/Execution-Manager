#ifndef __STATE__
#define __STATE__

#include <adaptive_app.hpp>
#include <application_state_client.h>

class State
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, std::string stateName);
    virtual ~State() = default;

    virtual std::unique_ptr<State> handleTransition() = 0;
    virtual void enter() = 0;
    virtual void leave() const;
protected:
    virtual api::ApplicationStateClient::ApplicationState getApplicationState() const = 0;
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
private:
    api::ApplicationStateClient::ApplicationState getApplicationState()  const override;
};

class Run : public State
{
public:
    Run(AdaptiveApp& app);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
private:
    api::ApplicationStateClient::ApplicationState getApplicationState() const override;
};

class Terminate : public State
{
public:
    Terminate(AdaptiveApp& app);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
private:
    api::ApplicationStateClient::ApplicationState getApplicationState() const override;
};

#endif
