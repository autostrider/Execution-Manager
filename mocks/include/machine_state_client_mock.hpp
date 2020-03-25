#ifndef MACHINE_STATE_CLIENT_MOCK_HPP
#define MACHINE_STATE_CLIENT_MOCK_HPP

#include <i_machine_state_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace api
{
using StateError = MachineStateClient::StateError;

class MachineStateClientMock : public IMachineStateClientWrapper
{
public:
    MachineStateClientMock() = default;
    MOCK_METHOD2(Register, StateError(std::string appName, std::uint32_t timeout));
    MOCK_METHOD2(GetMachineState, StateError(std::uint32_t timeout, std::string& state));
    MOCK_METHOD2(SetMachineState, StateError(std::string state, std::uint32_t timeout));
    MOCK_METHOD1(waitForConfirm, StateError(std::uint32_t timeout));
};

} // namespace api

#endif // MACHINE_STATE_CLIENT_MOCK_HPP