#include "machine_state_manager.hpp"

#include <algorithm>

namespace MachineStateManager
{

const vector<string> MachineStateManager::transitions =
    {MachineStates::kInit, MachineStates::kRunning, MachineStates::kShutdown};

void MachineStateManager::registerInExecutionManager()
{
    // TODO: send data about MachineStateManager to Execution manager
}

void MachineStateManager::handleStateConfirmation()
{
    MachineStates receivedState;
    sleep(5);
    auto newStatePos = std::find(transitions.begin(), transitions.end(), receivedState) + 1;
    if (newStatePos != transitions.end())
    {
        msc.setMachineState(*newStatePos);
    }
    else
}

} // namespace MachineStateManager
