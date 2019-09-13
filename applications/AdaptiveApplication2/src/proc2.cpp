#include <adaptive_app.hpp>
#include <state.hpp>

#include <signal.h>
#include <iostream>
#include <chrono>
#include <thread>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};
static const std::string appName{"AdaptiveApplication2.proc2"};

int main()
{
    if (::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cout << "[ " << appName << " ]:\tError while registering signal." << std::endl;
    }

    AdaptiveApp app(isTerminating, appName);

    while (true)
    {
        app.transitToNextState();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "[ " << appName << " ]:\tReceived signal: " 
              << sys_siglist[signo] << std::endl;

    isTerminating = true;
}