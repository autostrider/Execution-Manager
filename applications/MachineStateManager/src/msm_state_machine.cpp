#include <msm_state_machine.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

namespace MachineStateManager
{

MsmState::MsmState(MachineStateManager& msm,
             api::ApplicationStateClient::ApplicationState state,
             const std::string &stateName) :
    m_msm{msm}, m_msmState{state}, m_stateName{stateName}
{
        std::cout << "[ MachineStateManager ]:\tEnter " << m_stateName
                  << " state" << std::endl;
}

api::ApplicationStateClient::ApplicationState MsmState::getApplicationState() const
{
    return m_msmState;
}

Init::Init(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_INITIALIZING, "Initializing")
{
}

void Init::enter()
{
    std::cout << "[ MachineStateManager ]:\tReporting state Initializing..." << std::endl;
    m_msm.reportApplicationState(getApplicationState());

    std::cout << "[ MachineStateManager ]:\tMachine State Manager started.." << std::endl;

    const char* applicationName = "MachineStateManager";

    constexpr int defaulTimeout = 3000000;

    constexpr int timeForSleep = 5;

    StateError result = m_msm.registerMsm(applicationName, defaulTimeout);

    if (StateError::K_SUCCESS == result)
    {
        std::cout << "[ MachineStateManager ]:\tSuccessful registration as a MSM." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));
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

Run::Run(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_RUNNING, "Running")
{
}

void Run::enter()
{
    constexpr int defaulTimeout = 3000000;
    constexpr int timeForSleep = 5;

    std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to RUNNING..." << std::endl;

    m_msm.setMachineState("Running", defaulTimeout);

    std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to LIVING..." << std::endl;

    m_msm.setMachineState("Living", defaulTimeout);

    std::this_thread::sleep_for(std::chrono::seconds(timeForSleep));

    std::cout << "[ MachineStateManager ]:\tSetting machine state to SHUTTINGDOWN..." << std::endl;

    m_msm.setMachineState("Shuttingdown", defaulTimeout);
}

void Run::leave() const
{
}

Terminate::Terminate(MachineStateManager& msm) 
    : MsmState (msm, ApplicationState::K_SHUTTINGDOWN, "Terminating")
{
}

void Terminate::enter()
{
    std::cout << "[ MachineStateManager ]:\tReporting state "
              << m_stateName << "." << std::endl;

    std::cout << "[ MachineStateManager ]:\tIn Shutdown state." << std::endl
              << "[ MachineStateManager ]:\tIs dead..." << std::endl;

    m_msm.reportApplicationState(getApplicationState());

    ::exit(EXIT_SUCCESS);
}

void Terminate::leave() const
{
}

std::unique_ptr<api::IState> MsmStateFactory::createInit(api::IAdaptiveApp &msm)
{
    return std::make_unique<Init>(dynamic_cast<MachineStateManager&>(msm));
}

std::unique_ptr<api::IState> MsmStateFactory::createRun(api::IAdaptiveApp &msm)
{
    return std::make_unique<Run>(dynamic_cast<MachineStateManager&>(msm));
}

std::unique_ptr<api::IState> MsmStateFactory::createTerminate(api::IAdaptiveApp &msm)
{
    return std::make_unique<Terminate>(dynamic_cast<MachineStateManager&>(msm));
}

} // namespace MachineStateManager
