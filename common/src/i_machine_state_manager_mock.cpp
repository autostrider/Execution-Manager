#include "mocks/i_machine_state_manager_mock.hpp"

namespace MSM
{

IMachineStateManagerMock::IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
        std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
        std::unique_ptr<api::IMachineStateClientWrapper> machineClient,
        std::unique_ptr<ExecutionManager::IManifestReader> manifestReader)
    : MachineStateManager(std::move(factory),
                          std::move(appClient),
                          std::move(machineClient),
                          std::move(manifestReader))
{}

} // namespace MSM
