#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"
#include <logger.hpp>
#include <constants.hpp>
#include <manifest_reader.hpp>
#include "socket_server.hpp"
#include "socket_interface.hpp"
#include <i_application_state_client_wrapper.hpp>

#include <thread>
#include <atomic>

static void signalHandler(int signo);
using ApplicationState = api::ApplicationStateClient::ApplicationState;

static std::atomic<bool> isTerminated{false};

int main(int argc, char **argv)
{
    ::unlink(MSM_SOCKET_NAME.c_str());

    if (::signal(SIGTERM, signalHandler) == SIG_ERR
            ||
        ::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        LOG << "[msm] Error while registering signal.";
    }
    std::this_thread::sleep_for(FIVE_SECONDS);

    auto socket = std::make_unique<MSM::SocketInterface>();
    MSM::MachineStateManager msm(std::make_unique<MSM::MsmStateFactory>(),
                                 std::make_unique<api::ApplicationStateClientWrapper>(),
                                 std::make_unique<api::MachineStateClientWrapper>(),
                                 std::make_unique<ExecutionManager::ManifestReader>(),
                                 std::make_unique<MSM::SocketServer>(std::move(socket), MSM_STATES_SERVER));

    msm.init();
    msm.run();
    while (false == isTerminated)
    {
        msm.performAction();
        std::this_thread::sleep_for(FIVE_SECONDS);
    }

    msm.terminate();

    ::exit(EXIT_SUCCESS);
}

static void signalHandler(int signo)
{
    LOG << "[msm] Received signal:" << sys_siglist[signo];
    isTerminated = true;
}
