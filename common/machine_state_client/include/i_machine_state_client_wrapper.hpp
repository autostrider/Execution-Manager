#ifndef MACHINE_STATE_CLIENT_WRAPPER_H
#define MACHINE_STATE_CLIENT_WRAPPER_H

#include "mscForWrapper.hpp"

namespace machine_state_client
{

class IMachineStateClientWrapper
{
public:
    virtual ~IMachineStateClientWrapper() noexcept(false) {}

    virtual void setClient(std::unique_ptr<IClient> client) = 0;
    virtual StateError Register(std::string appName, uint32_t timeout) = 0;
    virtual StateError GetMachineState(std::string& state, uint32_t timeout) = 0;
    virtual StateError SetMachineState(std::string state, uint32_t timeout) = 0;
};

class MachineStateClientWrapper : public IMachineStateClientWrapper
{
public: 
    MachineStateClientWrapper(const std::string& path);

    void setClient(std::unique_ptr<IClient> client) override;
    StateError Register(std::string appName, uint32_t timeout) override;
    StateError GetMachineState(std::string& state, uint32_t timeout) override;
    StateError SetMachineState(std::string state, uint32_t timeout) override;

private:
    MscForWrapper m_machineClient;
};

}

#endif
