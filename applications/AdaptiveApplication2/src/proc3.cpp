#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <mean_calculator.hpp>
#include <i_application_state_client_wrapper.hpp>

#include <thread>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<ApplicationState> state{ApplicationState::K_INITIALIZING};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "[proc3] Error while registering signal.";
    }
    AdaptiveApp app3(std::make_unique<StateFactory>(),
                     std::make_unique<api::ApplicationStateClientWrapper>(),
                     std::make_unique<api::ComponentClientWrapper>("proc3"),
                     std::make_unique<MeanCalculator>());

    app3.init();

    state = ApplicationState::K_RUNNING;
    app3.run();

    while (ApplicationState::K_RUNNING == state)
    {
        app3.performAction();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    app3.terminate();

    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[proc3] Received signal: " << sys_siglist[signo] << ".";
    state = mapSignalToState.at(signo);
}
