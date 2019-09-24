#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>

#include <signal.h>
#include <iostream>
#include <thread>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal" << std::endl;
    }
    AdaptiveApp app(std::make_unique<StateFactory>(),
                    std::make_unique<api::ApplicationStateClientWrapper>());

    app.init();
    while (!isTerminating)
    {
        app.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    app.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << std::endl;
    isTerminating = true;
}
