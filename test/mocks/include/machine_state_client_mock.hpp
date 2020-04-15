#ifndef MACHINE_STATE_CLIENT_MOCK_HPP
#define MACHINE_STATE_CLIENT_MOCK_HPP

#include <i_machine_state_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace machine_state_client
{

class MachineStateClientMock : public IMachineStateClientWrapper
{
public:
    MachineStateClientMock() = default;
    MOCK_METHOD(StateError, Register, (std::string appName, std::uint32_t timeout), (override));
    MOCK_METHOD(StateError, GetMachineState, (std::string& state, uint32_t timeout), (override));
    MOCK_METHOD(StateError, SetMachineState, (std::string state, std::uint32_t timeout), (override));
    MOCK_METHOD(void, setClient, (std::unique_ptr<IClient> client), (override));
};

} // namespace machine_state_client


#endif // MACHINE_STATE_CLIENT_MOCK_HPP