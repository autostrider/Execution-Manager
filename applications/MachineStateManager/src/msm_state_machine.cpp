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
    std::cout << "[ MachineStateManager ]:\tEntergggggggggggggggg " << m_stateName
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
        std::cout<< "[ MachineStateManager ]:\tMust be shutdown." << std::endl;
        return std::make_unique<Shutdown>(m_msm);
    }
    return std::make_unique<Running>(m_msm);
}

void Init::enter()
{
    std::cout << "[ MachineStateManager ]:\tReporting state Initializing..." << std::endl;
    m_msm.reportApplicationState(getApplicationState());

    std::cout << "[ MachineStateManager ]:\tMachine State Manager started.." << std::endl;

    const char* applicationName = "MachineStateManager";
    constexpr int defaulTimeout = 300000;

    StateError result = m_msm.registerMsm(applicationName, defaulTimeout);

    if(StateError::K_SUCCESS == result)
    {
        std::cout << "[ MachineStateManager ]:\tSuccessful registration as a MSM." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    else
    {
        std::cerr << "[ MachineStateManager ]:\tUnsuccessful registration as a MSM." << std::endl
                  << "[ MachineStateManager ]:\tTerminating.." << std::endl;
    }
}

void Init::leave() const
{
    std::cout << "[ MachineStateManager ]:\tReporting state Running..." << std::endl;
    m_msm.reportApplicationState(ApplicationState::K_RUNNING);
}

//Application state running

Running::Running(MachineStateManager &msm)
    : State (msm, ApplicationState::K_RUNNING, "Running")
{
}

std::unique_ptr<State> Running::handleTransition()
{
    if (m_msm.isTerminating())
    {
        return std::make_unique<Shutdown>(m_msm);
    }

    return std::make_unique<Running>(m_msm);
}

void Running::enter()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to RUNNING..." << std::endl;
    m_msm.setMachineState("Running", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to LIVING..." << std::endl;
    m_msm.setMachineState("Living", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to SHUTTINGDOWN..." << std::endl;
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
        std::cout << "[ MachineStateManager ]:\tAlready in Shutdown state!" << std::endl;
    }
    else
    {
        std::cout << "[ MachineStateManager ]:\tEntering Shutdown state." << std::endl;
    }
    return std::make_unique<Shutdown>(m_msm);
}

void Shutdown::enter()
{
    std::cout << "[ MachineStateManager ]:\tReporting state "
              << m_stateName << "." << std::endl;

    std::cout << "[ MachineStateManager ]:\tIn Shutdown state." << std::endl
              << "[ MachineStateManager ]:\tIs dead..." << std::endl;

    m_msm.reportApplicationState(getApplicationState());

    ::exit(EXIT_SUCCESS);
}

}