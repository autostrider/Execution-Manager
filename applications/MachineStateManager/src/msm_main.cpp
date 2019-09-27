#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>

#include <signal.h>
#include <chrono>
#include <thread>
#include <atomic>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main(int argc, char **argv)
{
    ::unlink(MSM_SOCKET_NAME.c_str());

    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal";
    }

    MSM::MachineStateManager msm(std::make_unique<MSM::MsmStateFactory>(),
                                 std::make_unique<api::ApplicationStateClientWrapper>(),
                                 std::make_unique<api::MachineStateClientWrapper>());

    msm.init();

    while (!isTerminating)
    {
        msm.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    msm.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "Received signal:" << sys_siglist[signo];
    isTerminating = true;
}
