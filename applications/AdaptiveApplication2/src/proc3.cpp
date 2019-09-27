#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>

#include <csignal>
#include <thread>

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "[Proc3] Error while registering signal.";
    }

    AdaptiveApp app3(std::make_unique<StateFactory>(),
                     std::make_unique<api::ApplicationStateClientWrapper>());

    app3.init();

    while (!isTerminating)
    {
        app3.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    app3.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[proc3] Received signal: " << sys_siglist[signo] << ".";
    isTerminating = true;
}
