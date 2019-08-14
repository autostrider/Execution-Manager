#ifndef MACHINE_STATE_MANAGER_HPP
#define MACHINE_STATE_MANAGER_HPP

#include <machine_state_client.h>
#include <unistd.h>
#include <vector>

namespace MachineStateManager
{

using api::MachineStates;

using std::vector;

class MachineStateClient
{
public:
  void setMachineState(MachineStates state);
};

class MachineStateManager
{
public:
    void registerInExecutionManager();
    void start();
    void shutdown();
private:
    void handleStateConfirmation();

    MachineStateClient msc;
    const static vector<MachineStates> transitions;

};

} // namespace MachineStateManager

#endif // MACHINE_STATE_MANAGER_HPP
