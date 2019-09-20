#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <i_manifest_reader.hpp>
#include <i_application_handler.hpp>
#include "manifests.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ExecutionManager
{

using applicationId = std::string;
using MachineState = std::string;
using std::pair;

struct ApplicationManifest;

enum class AppState : uint16_t
{
  INITIALIZING,
  RUNNING,
  SHUTTINGDOWN
};

class ExecutionManager
{
public:
  ExecutionManager(std::unique_ptr<IManifestReader> reader,
                           std::unique_ptr<IApplicationHandler> applicationHandler);

  /**
   * @brief Main method of Execution manager.
   */
  void start();

  void reportApplicationState(pid_t processId, AppState state);

  bool registerMachineStateClient(pid_t processId, std::string appName);

  MachineState getMachineState(pid_t processId) const;

  bool setMachineState(pid_t processId, std::string state);
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

private:
  /**
   * @brief Hardcoded path to folder with adaptive applications.
   */
  const static std::string corePath;

  /**
   * @brief Holds interface responsible for starting applications
   */
  std::unique_ptr<IApplicationHandler> appHandler;

  /**
   * @brief structure that holds application and required processes.
   */
  std::map<MachineState, pid_t> m_activeApplications;

  /**
   * @brief Structure for application that can run in certain state
   * vector consists of applicationId (name) and string param - executable name.
   */
  std::map<MachineState, std::vector<ProcessInfo>> m_allowedApplicationForState;

  const static MachineState defaultState;

  /**
   * brief Current machine state.
   */
  MachineState m_currentState;

  /**
   * @brief Vector that holds state transitions.
   */
  std::vector<MachineState> m_machineManifestStates;

  std::string m_machineStateClientAppName;
  pid_t m_machineStateClientPid {-1};
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
