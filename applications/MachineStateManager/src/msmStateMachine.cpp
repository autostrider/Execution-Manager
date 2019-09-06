#include <chrono>
#include <thread>
#include <iostream>

#include <application_state_client.h>
#include "msmStateMachine.hpp"
#include "machine_state_manager.hpp"

using api::MachineStateClient;
namespace MachineStateManager{

//Application state init
std::unique_ptr<State> Init::handleTransition(MachineStateManager &msm)
{
    if (msm.isTerminating())
    {
        return std::make_unique<Shutdown>();
    }
    return std::make_unique<Running>();
}

void Init::enter(MachineStateManager &msm)
{
    std::cout << "MachineStateManager: Reporting state K_INITIALIZING..." << std::endl;
    msm.reportStateToEm(api::ApplicationStateClient::ApplicationState::K_INITIALIZING);

    std::cout << "MachineStateManager: Machine State Manager started.." << std::endl;

    const char* applicationName = "MachineStateManager";
    constexpr int defaulTimeout = 300000;

    MachineStateManager::StateError result = msm.registerMsm(applicationName, defaulTimeout);

    if(MachineStateManager::StateError::K_SUCCESS == result)
    {
        std::cout << "MachineStateManager: Successful registration as a MSM" << std::endl;
        
    }
    else
    {
        std::cerr << "MachineStateManager: Unsuccessful registration as a MSM.\nTerminating.." << std::endl;
    }
}

//Application state running
std::unique_ptr<State> Running::handleTransition(MachineStateManager &msm)
{
    return std::make_unique<Shutdown>();
}

void Running::enter(MachineStateManager &msm)
{
    std::cout << "MachineStateManager: Reporting state K_RUNNING..." << std::endl;
    msm.reportStateToEm(api::ApplicationStateClient::ApplicationState::K_RUNNING);

    std::cout << "MachineStateManager: Setting machine state to DRIVING..." <<std::endl;
    msm.setMachineState("Driving", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "MachineStateManager: Setting machine state to LIVING..." <<std::endl;
    msm.setMachineState("Living", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "MachineStateManager: Setting machine state to SHUTTINGDOWN..." <<std::endl;
    msm.setMachineState("Shuttingdown", 300000);
        
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

//Application state shutdown
std::unique_ptr<State> Shutdown::handleTransition(MachineStateManager &msm)
{
    if (msm.isTerminating())
    {
        std::cout << "MachineStateManager: Already in terminate state!";
    }
    else
    {
        std::cout << "MachineStateManager: Terminating state. App is dead(\n";
    }
    return std::make_unique<Shutdown>();
}

void Shutdown::enter(MachineStateManager &msm)
{
    std::cout << "MachineStateManager: Reporting state K_SHUTTINGDOWN..." << std::endl;
    msm.reportStateToEm(api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN);

    std::cout << "MachineStateManager: Enter terminate state" << std::endl
              << "MachineStateManager is SHUTTINGDOWN..." << std::endl;
    ::exit(EXIT_SUCCESS);
}

}