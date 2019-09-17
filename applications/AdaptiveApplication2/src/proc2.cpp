#include <adaptive_app.hpp>
#include <state.hpp>

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal" << std::endl;
    }

    AdaptiveApp app2(std::make_unique<StateFactory>(),
                     std::make_unique<api::ApplicationStateClientWrapper>());

    app2.init();

    while (!isTerminating)
    {
        app2.run();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    app2.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << std::endl;
    isTerminating = true;
}