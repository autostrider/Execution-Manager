#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>

#include <thread>
#include <atomic>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<ApplicationState> state{ApplicationState::K_INITIALIZING};

int main(int argc, char **argv)
{
    ::unlink(MSM_SOCKET_NAME.c_str());

    if (::signal(SIGTERM, signalHandler) == SIG_ERR
            ||
        ::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        LOG << "[msm] Error while registering signal.";
    }
    MSM::MachineStateManager msm(std::make_unique<MSM::MsmStateFactory>(),
                                 std::make_unique<api::ApplicationStateClientWrapper>(),
                                 std::make_unique<api::MachineStateClientWrapper>());

    const std::map<ApplicationState, StateHandler> dispatchMap
    {
        {ApplicationState::K_INITIALIZING, std::bind(&api::IAdaptiveApp::init, &msm)},
        {ApplicationState::K_RUNNING, std::bind(&api::IAdaptiveApp::run, &msm)},
        {ApplicationState::K_SHUTTINGDOWN, std::bind(&api::IAdaptiveApp::terminate, &msm)},
        {ApplicationState::K_SUSPEND, std::bind(&api::IAdaptiveApp::suspend, &msm)}
    };

    dispatchMap.at(state)();
    state = ApplicationState::K_RUNNING;

    while (ApplicationState::K_RUNNING == state)
    {
        dispatchMap.at(state)();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    dispatchMap.at(state)();

    ::exit(EXIT_SUCCESS);
}

static void signalHandler(int signo)
{
    LOG << "[msm] Received signal:" << sys_siglist[signo];
    state = mapSignalToState.at(signo);
}
