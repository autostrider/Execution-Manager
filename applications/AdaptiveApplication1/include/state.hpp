#ifndef STATE_HPP
#define STATE_HPP

#include <adaptive_app.hpp>
#include <i_state.hpp>
#include <i_state_factory.hpp>

class State : public api::IState
{
public:
    State(AdaptiveApp& app, application_state::ApplicationStateClient::ApplicationState state, std::string stateName);

    void leave() const override;
    virtual void performAction() override;
    application_state::ApplicationStateClient::ApplicationState getApplicationState() const;

protected:
    AdaptiveApp& m_app;
    const application_state::ApplicationStateClient::ApplicationState m_applState;
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

class Suspend : public api::IState
{
public:
    void enter() override;
    void leave() const override;
    virtual void performAction() override;
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
