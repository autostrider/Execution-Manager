#ifndef STATE_HPP
#define STATE_HPP

#include <adaptive_app.hpp>
#include <i_state.hpp>
#include <i_state_factory.hpp>

class State : public api::IState
{
public:
    State(AdaptiveApp& app, api::ApplicationStateClient::ApplicationState state, std::string stateName);

    virtual void enter() override = 0;
    void leave() const override;
    virtual void performAction();
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
};

class Run : public State
{
public:
    Run(AdaptiveApp& app);
    void enter() override;
    void performAction() override;
};

class ShutDown : public State
{
public:
    ShutDown(AdaptiveApp& app);
    void enter() override;
};

class Suspend : public State
{
public:
    Suspend(AdaptiveApp& app);
    void enter() override;
};

class StateFactory: public api::IStateFactory
{
public:
    std::unique_ptr<api::IState> createInit(api::IAdaptiveApp& app) const override;
    std::unique_ptr<api::IState> createRun(api::IAdaptiveApp& app) const override;
    std::unique_ptr<api::IState> createShutDown(api::IAdaptiveApp& app) const override;
    std::unique_ptr<api::IState> createSuspend(api::IAdaptiveApp& app) const override;
};

#endif // STATE_HPP
