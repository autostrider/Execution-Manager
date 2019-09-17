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
  : m_machineStateClient("unix:/tmp/execution_management")
{}

void MachineStateManager::start()
{
  m_appClient.ReportApplicationState
    (api::ApplicationStateClient::ApplicationState::K_INITIALIZING);

  if(m_machineStateClient.Register(applicationName, defaultTimeout) !=
     StateError::K_SUCCESS)
  {
    exit(EXIT_FAILURE);
  }

  m_appClient.ReportApplicationState
    (api::ApplicationStateClient::ApplicationState::K_RUNNING);

  std::cout << "Waiting for confirmation..." << std::endl;

  m_machineStateClient.waitForConfirm(defaultTimeout);

  std::cout << "Startup confirmed, setting up Running state..." << std::endl;

  m_machineStateClient.SetMachineState("Running", defaultTimeout);

  std::cout << "Running confirmed, waiting for 2 sec..." << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::cout << "Setting Shutdown state..." << std::endl;

  m_machineStateClient.SetMachineState("Shutdown", defaultTimeout);

  std::cout << "Shutdown state confirmed, exiting sucessfully!" << std::endl;

  m_appClient.ReportApplicationState
    (api::ApplicationStateClient::ApplicationState::K_SHUTTINGDOWN);

  exit(EXIT_SUCCESS);
}

} // namespace MachineStateManager