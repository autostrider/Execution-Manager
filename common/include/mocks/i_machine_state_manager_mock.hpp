#ifndef IMACHINESTATEMANAGERMOCK_HPP
#define IMACHINESTATEMANAGERMOCK_HPP

#include "machine_state_manager.hpp"

#include "gmock/gmock.h"

using namespace MSM;
using StateError = api::MachineStateClient::StateError;

class IMachineStateManagerMock : public MachineStateManager
{
public:
    IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
                             std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                             std::unique_ptr<api::IMachineStateClientWrapper> machineClient);

    MOCK_METHOD(void, init, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, terminate, ());

    MOCK_METHOD(void, reportApplicationState, (api::ApplicationStateClient::ApplicationState state));
    MOCK_METHOD(void, transitToNextState, (IMachineStateManagerMock::FactoryFunc nextState));
};

#endif // IMACHINESTATEMANAGERMOCK_HPP