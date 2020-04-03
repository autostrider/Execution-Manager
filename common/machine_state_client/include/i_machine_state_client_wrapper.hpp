#ifndef MACHINE_STATE_CLIENT_WRAPPER_H
#define MACHINE_STATE_CLIENT_WRAPPER_H

#include "machine_state_client.h"

namespace machine_state_client
{

class IMachineStateClientWrapper
{
public:
    virtual ~IMachineStateClientWrapper() noexcept(false) {}
    virtual StateError Register(std::string appName) = 0;
    virtual StateError GetMachineState(std::string& state) = 0;
    virtual StateError SetMachineState(std::string state) = 0;
};

class MachineStateClientWrapper : public IMachineStateClientWrapper
{
public: 
    MachineStateClientWrapper();
    StateError Register(std::string appName) override;
    StateError GetMachineState(std::string& state) override;
    StateError SetMachineState(std::string state) override;

private:
    MachineStateClient m_machineClient;
};

}

#endif
