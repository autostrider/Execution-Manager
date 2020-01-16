#ifndef MACHINE_STATE_CLIENT_WRAPPER_H
#define MACHINE_STATE_CLIENT_WRAPPER_H

#include <machine_state_client.h>
#include <machine_state_management.capnp.h>

namespace api {

class IMachineStateClientWrapper
{
public:
    virtual ~IMachineStateClientWrapper() noexcept(false) {}
    virtual MachineStateClient::StateError Register(std::string appName, std::uint32_t timeout) = 0;
    virtual MachineStateClient::StateError GetMachineState(std::uint32_t timeout, std::string& state) = 0;
    virtual MachineStateClient::StateError SetMachineState(std::string state, std::uint32_t timeout) = 0;
    virtual MachineStateClient::StateError waitForConfirm(std::uint32_t timeout) = 0;
};

class MachineStateClientWrapper : public IMachineStateClientWrapper
{
public: 
    MachineStateClientWrapper();
    MachineStateClient::StateError Register(std::string appName, std::uint32_t timeout) override;
    MachineStateClient::StateError GetMachineState(std::uint32_t timeout, std::string& state) override;
    MachineStateClient::StateError SetMachineState(std::string state, std::uint32_t timeout) override;
    MachineStateClient::StateError waitForConfirm(std::uint32_t timeout) override;

private:
    MachineStateClient m_machineClient;
};

}

#endif