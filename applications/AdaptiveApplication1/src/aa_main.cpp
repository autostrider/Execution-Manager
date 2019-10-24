#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <adaptive_app_base.hpp>

#include <thread>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<ApplicationState> state{ApplicationState::K_INITIALIZING};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "[aa_main] Error while registering signal.";
    }
    AdaptiveApp app(std::make_unique<StateFactory>(),
                    std::make_unique<api::ApplicationStateClientWrapper>(),
                    std::make_unique<api::ComponentClientWrapper>(),
                    std::make_unique<AdaptiveAppBase>());

    const std::map<ApplicationState, StateHandler> dispatchMap
    {
        {ApplicationState::K_INITIALIZING, std::bind(&api::IAdaptiveApp::init, &app)},
        {ApplicationState::K_RUNNING, std::bind(&api::IAdaptiveApp::run, &app)},
        {ApplicationState::K_SHUTTINGDOWN, std::bind(&api::IAdaptiveApp::terminate, &app)}
    };

    dispatchMap.at(state)();
    state = ApplicationState::K_RUNNING;

    while (ApplicationState::K_RUNNING == state)
    {
        dispatchMap.at(state)();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    dispatchMap.at(state)();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[aa_main] Received signal: " << sys_siglist[signo] << ".";
    state = mapSignalToState.at(signo);
}
