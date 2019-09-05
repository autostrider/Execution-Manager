#ifndef __STATE__
#define __STATE__

#include <adaptive_app.hpp>
#include <application_state_client.h>

class State
{
public:
    virtual ~State() = default;
    virtual std::unique_ptr<State> handleTransition(AdaptiveApp& app) = 0;
    virtual void enter(AdaptiveApp& app) = 0;
    virtual api::ApplicationStateClient::ApplicationState getStateName() = 0;
};

class Init : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
    api::ApplicationStateClient::ApplicationState getStateName() override;
};

class Run : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
    api::ApplicationStateClient::ApplicationState getStateName() override;
};

class Terminate : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
    api::ApplicationStateClient::ApplicationState getStateName() override;
};

#endif
