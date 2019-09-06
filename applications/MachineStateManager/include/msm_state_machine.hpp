#ifndef MSM_STATE_MACHINE_HPP
#define MSM_STATE_MACHINE_HPP

#include "machine_state_manager.hpp"

namespace MachineStateManager{
class MachineStateManager;
class State
{
public:
    virtual ~State() = default;
    virtual std::unique_ptr<State> handleTransition(MachineStateManager& msm) = 0;
    virtual void enter(MachineStateManager& msm) = 0;
};

class Init : public State
{
public:
    std::unique_ptr<State> handleTransition(MachineStateManager& msm) override;
    void enter(MachineStateManager& msm) override;
};

class Running : public State
{
public:
    std::unique_ptr<State> handleTransition(MachineStateManager& msm) override;
    void enter(MachineStateManager& msm) override;
};

class Shutdown : public State
{
public:
    std::unique_ptr<State> handleTransition(MachineStateManager& msm) override;
    void enter(MachineStateManager& msm) override;
};

}

#endif // MSM_STATE_MACHINE_HPP