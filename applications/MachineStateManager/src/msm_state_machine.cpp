#include <chrono>
#include <thread>
#include <iostream>

#include <application_state_client.h>
#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

namespace MachineStateManager{

State::State(MachineStateManager &msm, ApplicationState state, const std::string &stateName)
    : m_msm{msm}, m_applState{state}, m_stateName{stateName}
{
    std::cout << "Enter " << m_stateName
              << " state" << std::endl;
}

void State::leave() const
{
}

ApplicationState State::getApplicationState() const
{
    return m_applState;
}

//Application state init
Init::Init(MachineStateManager &msm)
    : State (msm, ApplicationState::K_INITIALIZING, "Initializing")
{
}

std::unique_ptr<State> Init::handleTransition()
{
    if (m_msm.isTerminating())
    {
        std::cout<< "Msm must be shutdown" << std::endl;
        return std::make_unique<Shutdown>(m_msm);
    }
    return std::make_unique<Running>(m_msm);
}

void Init::enter()
{
    std::cout << "[MachineStateManager]: Reporting state K_INITIALIZING..." << std::endl;
    m_msm.reportApplicationState(getApplicationState());

    std::cout << "[MachineStateManager]: Machine State Manager started.." << std::endl;

    const char* applicationName = "MachineStateManager";
    constexpr int defaulTimeout = 300000;

    StateError result = m_msm.registerMsm(applicationName, defaulTimeout);

    if(StateError::K_SUCCESS == result)
    {
        std::cout << "[MachineStateManager]: Successful registration as a MSM" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    else
    {
        std::cerr << "[MachineStateManager]: Unsuccessful registration as a MSM." << std::endl
                  << "Terminating.." << std::endl;
    }
}

void Init::leave() const
{
    m_msm.reportApplicationState(ApplicationState::K_RUNNING);
}

//Application state running

Running::Running(MachineStateManager &msm)
    : State (msm, ApplicationState::K_RUNNING, "Running")
{
}

std::unique_ptr<State> Running::handleTransition()
{
    return std::make_unique<Shutdown>(m_msm);
}

void Running::enter()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "[MachineStateManager]: Reporting state K_RUNNING..." << std::endl;
    m_msm.reportApplicationState(getApplicationState());

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[MachineStateManager]: Setting machine state to RUNNING..." << std::endl;
    m_msm.setMachineState("Running", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[MachineStateManager]: Setting machine state to LIVING..." << std::endl;
    m_msm.setMachineState("Living", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[MachineStateManager]: Setting machine state to SHUTTINGDOWN..." << std::endl;
    m_msm.setMachineState("Shuttingdown", 300000);
}

//Application state shutdown

Shutdown::Shutdown(MachineStateManager &msm)
    : State (msm, ApplicationState::K_SHUTTINGDOWN, "Shutdown")
{
}

std::unique_ptr<State> Shutdown::handleTransition()
{
    if (m_msm.isTerminating())
    {
        std::cout << "[MachineStateManager]: Already in terminate state!" << std::endl;
    }
    else
    {
        std::cout << "[MachineStateManager]: Terminating state. App is dead." << std::endl;
    }
    return std::make_unique<Shutdown>(m_msm);
}

void Shutdown::enter()
{
    std::cout << "[MachineStateManager]: Reporting state K_SHUTTINGDOWN..." << std::endl;
    m_msm.reportApplicationState(getApplicationState());

    std::cout << "[MachineStateManager]: Enter terminate state" << std::endl
              << "MachineStateManager is SHUTTINGDOWN..." << std::endl;

    ::exit(EXIT_SUCCESS);
}

}