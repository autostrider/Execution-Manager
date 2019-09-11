#ifndef STATE
#define STATE

#include <adaptive_app.hpp>
#include <application_state_client.h>

class State
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, const std::string& stateName);
    virtual ~State() = default;

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
    void enter() override;
    void leave() const override;
};

class Run : public State
{
public:
    Run(AdaptiveApp& app);
    void enter() override;
};

class Terminate : public State
{
public:
    Terminate(AdaptiveApp& app);
    void enter() override;
};


class AStateFactory
{
public:
    virtual std::unique_ptr<State> buildState(api::ApplicationStateClient::ApplicationState,
                                              AdaptiveApp &app) = 0;
    virtual~ AStateFactory() = default;
};

class StateFactory : public AStateFactory
{
public:
    std::unique_ptr<State> buildState(api::ApplicationStateClient::ApplicationState state,
                                      AdaptiveApp &app) override;
};
#endif
