#include <adaptive_app.hpp>
#include <state.hpp>

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <logger.hpp>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "Error while registering signal.";
    }
    AdaptiveApp app(std::make_unique<StateFactory>(),
                    std::make_unique<api::ApplicationStateClientWrapper>());

    app.init();
    while (!isTerminating)
    {
        app.run();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    app.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "Received signal:" << sys_siglist[signo] << ".";
    isTerminating = true;
}
