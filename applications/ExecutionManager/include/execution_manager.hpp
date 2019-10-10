#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "execution_manager_client.hpp"
#include <i_application_handler.hpp>
#include <i_manifest_reader.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace ExecutionManager
{

using applicationId = std::string;
using MachineState = std::string;
using ComponentState = std::string;
using ProcName = std::string;
using StateError = ::MachineStateManagement::StateError;
using ComponentClientReturnType = ::StateManagement::ComponentClientReturnType;
using std::pair;

struct ApplicationManifest;

enum class AppState : uint16_t
{
  INITIALIZING,
  RUNNING,
  SHUTTINGDOWN,
  SUSPEND
};

class ExecutionManager
{
public:
  ExecutionManager(std::unique_ptr<IManifestReader> reader,
                   std::unique_ptr<IApplicationHandler> applicationHandler,
                   std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client);

  /**
   * @brief Main method of Execution manager.
   */
  void start();

  void reportApplicationState(pid_t processId, AppState state);

  bool registerMachineStateClient(pid_t processId, std::string appName);

  MachineState getMachineState(pid_t processId) const;

  StateError setMachineState(pid_t processId, std::string state);

  ComponentState getComponentState(pid_t processId) const;

  void confirmComponentState
  (pid_t processId, std::string state, ComponentClientReturnType status);

private:
  /**
   * @brief Removes unsupported states from availApps
   */
  void filterStates();

  /**
   * @brief Starts given application and stores information
   *        about it in activeApplications.
   * @param process: Application to start.
   */
  void startApplication(const ProcessInfo& process);

  /**
   * @brief starts all application that support current state.
   */
  void startApplicationsForState();

  /**
   * @brief kill all processes that doesn't support current state.
   */
  void killProcessesForState();

  bool processToBeKilled (const std::string& app,
                          const std::vector<ProcessInfo>&);

  void confirmState(StateError status);

private:
  /**
   * @brief Holds interface responsible for starting applications
   */
  std::unique_ptr<IApplicationHandler> appHandler;

  /**
   * @brief structure that holds application and required processes.
   */
  std::map<ProcName, pid_t> m_activeProcesses;

  /**
   * @brief Structure for application that can run in certain state
   * vector consists of applicationId (name) and string param - executable name.
   */
  std::map<MachineState, std::vector<ProcessInfo>> m_allowedProcessesForState;

  const static MachineState defaultState;

  /**
   * brief Current machine state.
   */
  MachineState m_currentState;

  /**
   * brief Pending machine state.
   */
  MachineState m_pendingState;

  /**
   * @brief Vector that holds state transitions.
   */
  std::vector<MachineState> m_machineManifestStates;

  std::string m_machineStateClientAppName;
  pid_t m_machineStateClientPid {-1};

  std::set<pid_t> m_stateConfirmToBeReceived;

  std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> m_rpcClient;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
