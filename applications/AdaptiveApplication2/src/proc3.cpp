#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>

#include <csignal>
#include <thread>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<ApplicationState> state{ApplicationState::K_RUNNING};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR
            ||
        ::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        LOG << "[proc3] Error while registering signal.";
    }
    AdaptiveApp app3(std::make_unique<StateFactory>(),
                    std::make_unique<api::ApplicationStateClientWrapper>());

    app3.init();
    while (ApplicationState::K_RUNNING == state)
    {
        app3.run();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }
    switch (state)
    {
    case ApplicationState::K_SHUTTINGDOWN:
        app3.terminate();
        break;
    case ApplicationState::K_SUSPEND:
        app3.suspend();
        break;
    default:
        break;
    }

    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[proc3] Received signal: " << sys_siglist[signo] << ".";
    switch (signo)
    {
    case SIGTERM:
        state = ApplicationState::K_SHUTTINGDOWN;
        break;
    case SIGINT:
        state = ApplicationState::K_SUSPEND;
        break;
    default:
        LOG << "Received unsupported signal";

    }

}
