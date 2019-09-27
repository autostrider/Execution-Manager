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
        LOG << "[proc2] Error while registering signal.";
    }

    AdaptiveApp app2(std::make_unique<StateFactory>(),
                     std::make_unique<api::ApplicationStateClientWrapper>());

    app2.init();

    while (!isTerminating)
    {
        app2.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    app2.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[proc2] Received signal: " << sys_siglist[signo] << ".";
    isTerminating = true;
}
