#include <iostream>
#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include "logger.hpp"

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main(int argc, char **argv)
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal";
    }

    MachineStateManager::MachineStateManager msm(std::make_unique<MachineStateManager::MsmStateFactory>(),
                                                 std::make_unique<api::ApplicationStateClientWrapper>());

    msm.init();

    while (!isTerminating)
    {
        msm.run();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    msm.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "Received signal:" << sys_siglist[signo];
    isTerminating = true;
}
