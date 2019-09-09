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

    try {
        AdaptiveApp app(isTerminating);
        app.init(std::make_unique<Init>(app),
                 std::make_unique<api::ApplicationStateClient>());
        while (true)
        {
            app.transitToNextState();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        ::exit(EXIT_SUCCESS);
    }
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    isTerminating = true;
}
