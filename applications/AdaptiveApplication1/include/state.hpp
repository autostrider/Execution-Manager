#ifndef __STATE__
#define __STATE__

#include <adaptive_app.hpp>

class State
{
public:
    virtual ~State() = default;
    virtual std::unique_ptr<State> handleTransition(AdaptiveApp& app) = 0;
    virtual void enter(AdaptiveApp& app) = 0;
};

class Init : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
};

class Run : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
};

class Terminate : public State
{
public:
    std::unique_ptr<State> handleTransition(AdaptiveApp& app) override;
    void enter(AdaptiveApp& app) override;
};

#endif
