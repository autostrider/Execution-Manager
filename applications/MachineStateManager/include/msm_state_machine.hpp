#ifndef MSM_STATE_MACHINE_HPP
#define MSM_STATE_MACHINE_HPP

#include "machine_state_manager.hpp"
#include <i_state_factory.hpp>

namespace api
{
    class ISocketServer;
}

namespace MSM
{

class MsmState : public api::IState
{
public:
    MsmState(MachineStateManager& msm, 
             application_state::ApplicationStateClient::ApplicationState state, 
             std::string stateName);

    virtual void enter() = 0;
    void leave() const override;
    void performAction() override;
    application_state::ApplicationStateClient::ApplicationState getApplicationState() const;

protected:
    MachineStateManager& m_msm;
    const application_state::ApplicationStateClient::ApplicationState m_msmState;
    const std::string m_stateName;
};

class Init : public MsmState
{
public:
    Init(MachineStateManager& msm);
    void enter() override;
    void leave() const override;
};

class Run : public MsmState
{
public:
    Run(MachineStateManager& msm);
    void enter() override;
};

class ShutDown : public MsmState
{
public:
    ShutDown(MachineStateManager& msm);
    void enter() override;
};

class MsmStateFactory : public api::IStateFactory
{
public:
    std::unique_ptr<api::IState> createInit(api::IAdaptiveApp& msm) const override;
    std::unique_ptr<api::IState> createRun(api::IAdaptiveApp& msm) const override;
    std::unique_ptr<api::IState> createShutDown(api::IAdaptiveApp& msm) const override;
    std::unique_ptr<api::IState> createSuspend(api::IAdaptiveApp& msm) const override;
};

} // namespace MSM
#endif // MSM_STATE_MACHINE_HPP
