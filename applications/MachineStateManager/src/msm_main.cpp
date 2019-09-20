#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"
#include <logger.hpp>

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main(int argc, char **argv)
{
  const char* socketName = "/tmp/machine_management";
  ::unlink(socketName);

    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal";
    }

    MSM::MachineStateManager msm(std::make_unique<MSM::MsmStateFactory>(),
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
