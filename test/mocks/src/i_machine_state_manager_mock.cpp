#include "i_machine_state_manager_mock.hpp"
#include <i_application_state_client_wrapper.hpp>
#include <i_manifest_reader.hpp>
#include <i_state_factory.hpp>
#include <i_socket_server.hpp>
#include <keyvaluestorage.h>

namespace MSM
{

IMachineStateManagerMock::IMachineStateManagerMock(
        std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<application_state::IApplicationStateClientWrapper> appClient,
        std::unique_ptr<machine_state_client::IMachineStateClientWrapper> machineClient,
        std::unique_ptr<api::IManifestReader> manifestReader,
        std::unique_ptr<api::ISocketServer> socketServer,
        std::unique_ptr<per::KeyValueStorageBase> persistentStorage)
    : MachineStateManager(std::move(factory),
                          std::move(appClient),
                          std::move(machineClient),
                          std::move(manifestReader),
                          std::move(socketServer),
                          std::move(persistentStorage))
{}

} // namespace MSM
