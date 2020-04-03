#include "i_machine_state_client_wrapper.hpp"
#include <constants.hpp>

using namespace constants;

namespace machine_state_client
{

MachineStateClientWrapper::MachineStateClientWrapper() : m_machineClient{IPC_PROTOCOL + EM_SOCKET_NAME}
{}

StateError MachineStateClientWrapper::Register(std::string appName)
{
    return m_machineClient.Register(appName);
}

StateError MachineStateClientWrapper::GetMachineState(std::string& state)
{
    return m_machineClient.GetMachineState(state);
}

StateError MachineStateClientWrapper::SetMachineState(std::string state)
{
    return m_machineClient.SetMachineState(state);
}

}
