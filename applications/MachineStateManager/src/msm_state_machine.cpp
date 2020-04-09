#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>

#include <thread>

using namespace constants;

using ApplicationState = application_state::ApplicationStateClient::ApplicationState;
using StateError = machine_state_client::StateError;

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
    : MsmState (msm, ApplicationState::kInitializing, AA_STATE_INIT)
{
}

void Init::enter()
{
    LOG << "Reporting state "
        << m_stateName << ".";

    m_msm.reportApplicationState(getApplicationState());

    LOG << "Machine State Manager started...";

    std::string applicationName{"MSM"};

    StateError result = m_msm.registerMsm(applicationName);

    if (StateError::kSuccess == result)
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

    m_msm.reportApplicationState(ApplicationState::kRunning);
}

Run::Run(MachineStateManager& msm)
    : MsmState (msm, ApplicationState::kRunning, AA_STATE_RUNNING)
{
}

void Run::enter()
{
  std::string state;
  do
  {
    state = m_msm.getNewState();
    m_msm.saveReceivedState(state);

    LOG << "Setting machine state to "
        << state
        << "...";

    StateError result = m_msm.setMachineState(state);

    if (StateError::kSuccess != result)
    {
        if (!((StateError::kTimeout == result) && (MACHINE_STATE_SHUTTINGDOWN == state)))
        {
            LOG << "Failed to set machine state " << state << ".";
        }
    }
  } while (state != MACHINE_STATE_SHUTTINGDOWN);
}

ShutDown::ShutDown(MachineStateManager& msm)
    : MsmState (msm, ApplicationState::kShuttingdown, AA_STATE_SHUTDOWN)
{
}

void ShutDown::enter()
{
    LOG << "Reporting state "
        << m_stateName << ".";
    m_msm.reportApplicationState(getApplicationState());
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
    return nullptr;
}

} // namespace MSM
