#include <adaptive_app.hpp>
#include <state.hpp>

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <logger.hpp>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

namespace
{
    constexpr int timeout = 5;
}

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal" << std::endl;
    }

    AdaptiveApp app2(std::make_unique<StateFactory>(),
                     std::make_unique<api::ApplicationStateClientWrapper>());

    app2.init();

    while (!isTerminating)
    {
        app2.run();
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
    }
    app2.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "received signal:" << sys_siglist[signo] << std::endl;
    isTerminating = true;
}