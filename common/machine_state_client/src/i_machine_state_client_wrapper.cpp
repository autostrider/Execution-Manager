#include "i_machine_state_client_wrapper.hpp"

namespace machine_state_client
{

MachineStateClientWrapper::MachineStateClientWrapper(const std::string& path)
    : m_machineClient(path)
{}

void MachineStateClientWrapper::setClient(std::unique_ptr<IClient> client)
{
    m_machineClient.setClient(std::move(client));
}

StateError MachineStateClientWrapper::Register(std::string appName, uint32_t timeout)
{
    return m_machineClient.Register(appName, timeout);
}

StateError MachineStateClientWrapper::GetMachineState(std::string& state, uint32_t timeout)
{
    return m_machineClient.GetMachineState(state, timeout);
}

StateError MachineStateClientWrapper::SetMachineState(std::string state, uint32_t timeout)
{
    return m_machineClient.SetMachineState(state, timeout);
}

}
