#ifndef STATE
#define STATE

#include <adaptive_app.hpp>
#include <application_state_client.h>

class IState
{
public:
    virtual ~IState() = default;
    virtual void enter() = 0;
    virtual void leave() const;
};
class State : public IState
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, const std::string& stateName);

    virtual void enter() = 0;
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


class IStateFactory
{
public:
    virtual std::unique_ptr<IState> makeInit(AdaptiveApp &app) = 0;
    virtual std::unique_ptr<IState> makeRun(AdaptiveApp &app) = 0;
    virtual std::unique_ptr<IState> makeTerminate(AdaptiveApp &app) = 0;

    virtual~ IStateFactory() = default;
};

class StateFactory : public IStateFactory
{
public:
    std::unique_ptr<IState> makeInit(AdaptiveApp &app) override;
    std::unique_ptr<IState> makeRun(AdaptiveApp &app) override;
    std::unique_ptr<IState> makeTerminate(AdaptiveApp &app) override;
};
#endif
