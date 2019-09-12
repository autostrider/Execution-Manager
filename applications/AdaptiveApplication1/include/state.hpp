#ifndef STATE_HPP
#define STATE_HPP

#include <adaptive_app.hpp>
#include <i_state.hpp>
#include <i_state_factory.hpp>

class State : public api::IState
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, const std::string& stateName);

    virtual void enter() = 0;
    void leave() const override;
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


class StateFactory: public api::IStateFactory
{
public:
    std::unique_ptr<api::IState> createInit(api::IAdaptiveApp &app) override;
    std::unique_ptr<api::IState> createRun(api::IAdaptiveApp &app) override;
    std::unique_ptr<api::IState> createTerminate(api::IAdaptiveApp &app) override;
};

#endif // STATE_HPP
