#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <machine_state_client.h>
#include <vector>

namespace MachineStateManager
{

using std::vector;

using api::MachineStates;

class MachineStateClient
{
public:
    void setMachineState(MachineStates state);
};

class MachineStateManager
{
public:
    void registerInExecutionManager();
    void work();
    const static vector<MachineStates> transition;
private:
    MachineStateClient msc;
};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP
