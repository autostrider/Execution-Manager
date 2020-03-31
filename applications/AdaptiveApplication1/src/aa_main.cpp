#include "adaptive_app.hpp"
#include <state.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <mean_calculator.hpp>
#include <i_application_state_client_wrapper.hpp>
#include <component_server.hpp>
#include <connection_factory.hpp>
#include <proxy_client_factory.hpp>
#include <server_socket.hpp>

#include <csignal>
#include <thread>

static void signalHandler(int signo);
using ApplicationState = application_state::ApplicationStateClient::ApplicationState;
using namespace base_client;

static std::atomic<bool> isTerminated{false};

int main()
{
    if (::signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        LOG << "[aa_main] Error while registering signal.";
    }

    std::this_thread::sleep_for(FIVE_SECONDS);

    const std::string componentName{"AdaptiveApplication1_proc1"};
    auto componentStateUpdateMode = component_client::StateUpdateMode::K_EVENT;

    AdaptiveApp app(std::make_unique<StateFactory>(),
                    std::make_unique<application_state::ApplicationStateClientWrapper>(),
                    std::make_unique<component_client::ComponentClientWrapper>(componentName,
                                                                               componentStateUpdateMode),
                    std::make_unique<component_server::ComponentServer>((IPC_PROTOCOL + COMPONENT_SOCKET_NAME + componentName),
                                                                         std::make_unique<socket_handler::ServerSocket>(),
                                                                         std::make_unique<ConnectionFactory>(std::make_shared<ProxyClientFactory>())),
                    std::make_unique<api::MeanCalculator>(),
                    true);


    app.init();

    app.run();
    while (false == isTerminated)
    {
        app.performAction();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    app.terminate();
    return 0;
}

static void signalHandler(int signo)
{
    LOG << "[aa_main] Received signal: " << sys_siglist[signo] << ".";
    isTerminated = true;
}
