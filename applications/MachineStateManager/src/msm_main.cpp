#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>

#include <signal.h>
#include <chrono>
#include <thread>
#include <atomic>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<ApplicationState> state{ApplicationState::K_RUNNING};

int main(int argc, char **argv)
{
    ::unlink(MSM_SOCKET_NAME.c_str());

    if (::signal(SIGTERM, signalHandler) == SIG_ERR
            ||
        ::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal.";
    }
    MSM::MachineStateManager msm(std::make_unique<MSM::MsmStateFactory>(),
                                 std::make_unique<api::ApplicationStateClientWrapper>(),
                                 std::make_unique<api::MachineStateClientWrapper>());

    msm.init();

    while (ApplicationState::K_RUNNING == state)
    {
        msm.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    switch (state)
    {
    case ApplicationState::K_SHUTTINGDOWN:
        msm.terminate();
        break;
    case ApplicationState::K_SUSPEND:
        msm.suspend();
        break;
    default:
        break;
    }

    return 0;
}

static void signalHandler(int signo)
{
    LOG << "Received signal:" << sys_siglist[signo];
    switch (signo)
    {
    case SIGTERM:
        state = ApplicationState::K_SHUTTINGDOWN;
        break;
    case SIGINT:
        state = ApplicationState::K_SUSPEND;
        break;
    default:
        LOG << "Received unsupported signal";

    }
}
