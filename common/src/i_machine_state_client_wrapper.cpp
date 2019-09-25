#include "i_machine_state_client_wrapper.hpp"
#include <constants.hpp>

namespace api {

using StateError = MachineStateClient::StateError;

MachineStateClientWrapper::MachineStateClientWrapper() : m_machineClient{EM_SOCKET_NAME}
{}

StateError MachineStateClientWrapper::Register(std::string appName, std::uint32_t timeout)
{
    return m_machineClient.Register(appName, timeout);
}

StateError MachineStateClientWrapper::GetMachineState(std::uint32_t timeout, std::string& state)
{
    return m_machineClient.GetMachineState(timeout, state);
}

StateError MachineStateClientWrapper::SetMachineState(std::string state, std::uint32_t timeout)
{
    return m_machineClient.SetMachineState(state, timeout);
}

StateError MachineStateClientWrapper::waitForConfirm(std::uint32_t timeout)
{
    return m_machineClient.waitForConfirm(timeout);
}

}