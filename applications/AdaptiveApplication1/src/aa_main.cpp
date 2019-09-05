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
    if (::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
    }
    AdaptiveApp app(isTerminating);

    while (true)
    {
        app.transitToNextState();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    isTerminating = true;
}
