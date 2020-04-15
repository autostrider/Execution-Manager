#ifndef I_MACHINE_STATE_MANAGER_MOCK_HPP
#define I_MACHINE_STATE_MANAGER_MOCK_HPP

#include <machine_state_manager.hpp>

#include "gmock/gmock.h"

namespace MSM
{
class IMachineStateManagerMock : public MachineStateManager
{
public:
    IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
                             std::unique_ptr<application_state::IApplicationStateClientWrapper> appStateClient,
                             std::unique_ptr<machine_state_client::IMachineStateClientWrapper> machineClient,
                             std::unique_ptr<api::IManifestReader> manifestReader,
                             std::unique_ptr<api::IClient> cilent,
                             std::unique_ptr<per::KeyValueStorageBase> persistentStorage);

    MOCK_METHOD(void, init, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, terminate, ());

    MOCK_METHOD(void, reportApplicationState, (application_state::ApplicationStateClient::ApplicationState));
    MOCK_METHOD(void, transitToNextState, (IMachineStateManagerMock::FactoryFunc));
};

} // namespace MSM

#endif // I_MACHINE_STATE_MANAGER_MOCK_HPP
