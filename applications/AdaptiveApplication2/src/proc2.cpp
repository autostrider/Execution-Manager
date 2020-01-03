#include <adaptive_app.hpp>
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <mean_calculator.hpp>
#include <i_application_state_client_wrapper.hpp>


#include <thread>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<bool> isTerminated{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "[proc2] Error while registering signal.";
    }

    const std::string componentName{"AdaptiveApplication2_proc2"};
    auto componentStateUpdateMode = api::StateUpdateMode::K_POLL;

    AdaptiveApp app2(std::make_unique<StateFactory>(),
                    std::make_unique<api::ApplicationStateClientWrapper>(),
                    std::make_unique<api::ComponentClientWrapper>(componentName,
                                                                  componentStateUpdateMode),
                    std::make_unique<MeanCalculator>());

    app2.init();
    app2.run();

    while (false == isTerminated)
    {
        app2.performAction();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    std::this_thread::sleep_for(2*FIVE_SECONDS);
    app2.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[proc2] Received signal: " << sys_siglist[signo] << ".";
    isTerminated = true;
}
