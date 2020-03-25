#ifndef I_MACHINE_STATE_MANAGER_MOCK_HPP
#define I_MACHINE_STATE_MANAGER_MOCK_HPP

#include <machine_state_manager.hpp>

#include "gmock/gmock.h"

namespace MSM
{

using StateError = machine_state_client::MachineStateClient::StateError;

class IMachineStateManagerMock : public MachineStateManager
{
public:
    IMachineStateManagerMock(
            std::unique_ptr<api::IStateFactory> factory,
            std::unique_ptr<application_state::IApplicationStateClientWrapper> appClient,
            std::unique_ptr<machine_state_client::IMachineStateClientWrapper> machineClient,
            std::unique_ptr<api::IManifestReader> manifestReader,
            std::unique_ptr<api::ISocketServer> socketServer,
            std::unique_ptr<per::KeyValueStorageBase> persistentStorage);

    MOCK_METHOD0(init, void());
    MOCK_METHOD0(run, void());
    MOCK_METHOD0(terminate, void());

    MOCK_METHOD1(reportApplicationState, void(application_state::ApplicationStateClient::ApplicationState state));
    MOCK_METHOD1(transitToNextState, void(IMachineStateManagerMock::FactoryFunc nextState));
};

} // namespace MSM

#endif // I_MACHINE_STATE_MANAGER_MOCK_HPP
