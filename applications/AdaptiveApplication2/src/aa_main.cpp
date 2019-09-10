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
        std::cout << "[ AdaptiveApplication2 ]:\tError while registering signal." << std::endl;
    }
    AdaptiveApp app(isTerminating);

    while (true)
    {
        app.transitToNextState();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "[ AdaptiveApplication2 ]:\tReceived signal: "
              << sys_siglist[signo] << std::endl;
    isTerminating = true;
}