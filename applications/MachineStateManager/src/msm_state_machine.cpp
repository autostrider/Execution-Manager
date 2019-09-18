#include "msm_state_machine.hpp"
#include <logger.hpp>

#include <chrono>
#include <thread>
#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

namespace
{
    constexpr uint defaulTimeout = 3000000;
    constexpr int timeForSleep = 5;
}

namespace MachineStateManager
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

Init::Init(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_INITIALIZING, "Initializing")
{
}

void Init::enter()
{
    LOG << "Reporting state "
        << m_stateName << ".";
    
    m_msm.reportApplicationState(getApplicationState());

    LOG << "Machine State Manager started...";

    std::string applicationName{"MachineStateManager"};

    StateError result = m_msm.registerMsm(applicationName, defaulTimeout);

    if (StateError::K_SUCCESS == result)
    {
        LOG << "Successful registration as a MSM.";
        std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));
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
    : MsmState (msm, ApplicationState::K_RUNNING, "Running")
{
}

void Run::enter()
{
    const std::vector<std::string> states{"Running", "Living", "Shuttingdown"};
  
    for (auto& state : states)
    {
      std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));

      LOG << "Setting machine state to " 
          << state 
          << "...";

      m_msm.setMachineState(state, defaulTimeout);     
    }
}

ShutDown::ShutDown(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_SHUTTINGDOWN, "Terminating")
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

} // namespace MachineStateManager
