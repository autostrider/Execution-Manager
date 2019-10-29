#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>

#include <thread>

using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

namespace MSM
{

MsmState::MsmState(MachineStateManager& msm,
                   ApplicationState state,
                   std::string stateName) :
                   m_msm{msm}, 
                   m_msmState{state}, 
                   m_stateName{std::move(stateName)}
{
    LOG << "Enter " << m_stateName << " state.";
}

ApplicationState MsmState::getApplicationState() const
{
    return m_msmState;
}

void MsmState::leave() const
{
}

void MsmState::performAction()
{

}

Init::Init(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_INITIALIZING, AA_STATE_INIT)
{
}

void Init::enter()
{
    LOG << "Reporting state "
        << m_stateName << ".";
    
    m_msm.reportApplicationState(getApplicationState());

    LOG << "Machine State Manager started...";

    std::string applicationName{"MachineStateManager"};

    StateError result = m_msm.registerMsm(applicationName);

    if (StateError::K_SUCCESS == result)
    {
        LOG << "Successful registration as a MSM.";
        std::this_thread::sleep_for(FIVE_SECONDS);
    } 
    else
    {
        LOG << "Unsuccessful registration as a MSM. Terminating...";
    }
}

void Init::leave() const
{
    LOG << "Reporting state "
        << m_stateName << ".";

    m_msm.reportApplicationState(ApplicationState::K_RUNNING);
}

Run::Run(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_RUNNING, AA_STATE_RUNNING)
{
}

void Run::enter()
{
    const std::vector<std::string> states{MACHINE_STATE_RUNNING,
                                          MACHINE_STATE_LIVING,
                                          MACHINE_STATE_SHUTTINGDOWN};
  
    StateError result;

    for (auto& state : states)
    {
        std::this_thread::sleep_for(FIVE_SECONDS);

        LOG << "Setting machine state to " 
            << state 
            << "...";

        result = m_msm.setMachineState(state);

        if (StateError::K_SUCCESS != result)
        {
            if (!((StateError::K_TIMEOUT == result) && (MACHINE_STATE_SHUTTINGDOWN == state)))
            {         
                LOG << "Failed to set machine state " << state << ".";
            }
        }
    }
}

ShutDown::ShutDown(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_SHUTTINGDOWN, AA_STATE_SHUTDOWN)
{
}

void ShutDown::enter()
{
    LOG << "Reporting state "
        << m_stateName << ".";

    m_msm.reportApplicationState(getApplicationState());
}

Suspend::Suspend(MachineStateManager &msm) : MsmState (msm, ApplicationState::K_SUSPEND, AA_STATE_SUSPEND)
{

}

void Suspend::enter()
{

}

std::unique_ptr<api::IState> MsmStateFactory::createInit(api::IAdaptiveApp& msm) const
{
    return std::make_unique<Init>(dynamic_cast<MachineStateManager&>(msm));
}

std::unique_ptr<api::IState> MsmStateFactory::createRun(api::IAdaptiveApp& msm) const
{
    return std::make_unique<Run>(dynamic_cast<MachineStateManager&>(msm));
}

std::unique_ptr<api::IState> MsmStateFactory::createShutDown(api::IAdaptiveApp& msm) const
{
    return std::make_unique<ShutDown>(dynamic_cast<MachineStateManager&>(msm));
}

std::unique_ptr<api::IState> MsmStateFactory::createSuspend(api::IAdaptiveApp &msm) const
{
    return std::make_unique<Suspend>(dynamic_cast<MachineStateManager&>(msm));
}

} // namespace MSM
