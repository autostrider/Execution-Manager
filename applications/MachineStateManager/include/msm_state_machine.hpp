#ifndef MSM_STATE_MACHINE_HPP
#define MSM_STATE_MACHINE_HPP

namespace MachineStateManager{

class MachineStateManager;

class State
{
public:
    State(MachineStateManager& msm,
          api::ApplicationStateClient::ApplicationState state,
          const std::string& stateName);
    virtual ~State() = default;

    virtual std::unique_ptr<State> handleTransition() = 0;
    virtual void enter() = 0;
    virtual void leave() const;

protected:
    api::ApplicationStateClient::ApplicationState getApplicationState() const;
    MachineStateManager& m_msm;
    const api::ApplicationStateClient::ApplicationState m_applState;
    const std::string m_stateName;
};

class Init : public State
{
public:
    Init(MachineStateManager& m_msm);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
    void leave() const override;
};

class Running : public State
{
public:
    Running(MachineStateManager& m_msm);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

class Shutdown : public State
{
public:
    Shutdown(MachineStateManager& m_msm);
    std::unique_ptr<State> handleTransition() override;
    void enter() override;
};

}

#endif // MSM_STATE_MACHINE_HPP