#include "mocks/i_machine_state_manager_mock.hpp"
#include "i_manifest_reader.hpp"
#include "i_state_factory.hpp"
#include "i_socket_server.hpp"
#include "i_application_state_client_wrapper.hpp"

namespace MSM
{

IMachineStateManagerMock::IMachineStateManagerMock(
        std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
        std::unique_ptr<api::IMachineStateClientWrapper> machineClient,
        std::unique_ptr<ExecutionManager::IManifestReader> manifestReader,
        std::unique_ptr<ISocketServer> socketServer)
    : MachineStateManager(std::move(factory),
                          std::move(appClient),
                          std::move(machineClient),
                          std::move(manifestReader),
                          std::move(socketServer))
{}

} // namespace MSM
