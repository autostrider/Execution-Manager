#include "machine_state_manager.hpp"

#include <thread>

namespace MachineStateManager {

const vector<MachineStates> MachineStateManager::transition =
        {MachineStates::kInit, MachineStates::kRunning, MachineStates::kShutdown};

void MachineStateManager::registerInExecutionManager()
{

}

void MachineStateManager::work()
{
    for(auto requiedState = transition.begin() + 1; requiedState != transition.end(); requiedState++)
    {
        std::this_thread::sleep_for(std::chrono::seconds{3});
        msc.setMachineState(*requiedState);
    }
}

} // namespace MachineStateManager
