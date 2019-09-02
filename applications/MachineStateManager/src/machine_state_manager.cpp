#include <chrono>
#include <thread>

#include "machine_state_manager.hpp"

namespace MachineStateManager {

using api::MachineStateClient;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

MachineStateManager::MachineStateManager()
  : machineStateClient(std::make_unique<MachineStateClient>
                        ("unix:/tmp/execution_management"))
{}

int32_t MachineStateManager::start()
{
  cout << "MachineStateManager: Machine State Manager started.." << endl;

  const char* applicationName = "MachineStateManager";
  constexpr int defaulTimeout = 300000;

  StateError result =
    machineStateClient->Register(applicationName, defaulTimeout);

  if(StateError::K_SUCCESS == result)
  {
    cout << "MachineStateManager: Successful registration as a MSM" << endl;
  }
  else
  {
    cerr << "MachineStateManager: Unsuccessful registration as a MSM.\nTerminating.." << endl;
    return EXIT_FAILURE;
  }

  while (1)
  {
    std::cout << "MachineStateManager: Setting machine state to INIT..." <<std::endl;
    result =
      machineStateClient->SetMachineState("init", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "MachineStateManager: Setting machine state to RUNNING..." <<std::endl;
    result =
      machineStateClient->SetMachineState("running", 300000);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "MachineStateManager: Setting machine state to SHUTDOWN..." <<std::endl;
    result =
      machineStateClient->SetMachineState("shutdown", 300000);
    
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }

  return EXIT_SUCCESS;
}

} // namespace MachineStateManager