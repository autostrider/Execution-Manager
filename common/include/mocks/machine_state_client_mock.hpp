#ifndef MACHINE_STATE_CLIENT_MOCK_HPP
#define MACHINE_STATE_CLIENT_MOCK_HPP

#include "i_machine_state_client_wrapper.hpp"

#include "gmock/gmock.h"

namespace api
{
using StateError = MachineStateClient::StateError;

class MachineStateClientMock : public IMachineStateClientWrapper
{
public:
    MachineStateClientMock() = default;
    MOCK_METHOD(StateError, Register, (std::string appName, std::uint32_t timeout));
    MOCK_METHOD(StateError, GetMachineState, (std::uint32_t timeout, std::string& state));
    MOCK_METHOD(StateError, SetMachineState, (std::string state, std::uint32_t timeout));
    MOCK_METHOD(StateError, waitForConfirm, (std::uint32_t timeout));
};

} // namespace api

#endif // MACHINE_STATE_CLIENT_MOCK_HPP