#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "execution_manager_client.hpp"
#include "imanifest_reader.hpp"
#include "manifests.hpp"
#include <machine_state_management.capnp.h>

#include <chrono>
#include <csignal>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <set>
#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>

namespace ExecutionManager
{

using applicationId = std::string;
using MachineState = std::string;
using ProcName = std::string;
using StateError = ::MachineStateManagement::StateError;
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

  explicit ExecutionManager(
    std::unique_ptr<IManifestReader> reader,
    std::unique_ptr<ExecutionManagerClient::ExecutionManagerClient> client);

  /**
   * @brief Main method of Execution manager.
   */
  void start();

  void reportApplicationState(pid_t processId, AppState state);

  bool registerMachineStateClient(pid_t processId, std::string appName);

  MachineState getMachineState(pid_t processId) const;

  StateError setMachineState(pid_t processId, std::string state);
private:

  /**
   * @brief Removes unsupported states from availApps
   */
  void filterStates();

  /**
   * @brief Builds vector of command line arguments passed to application.
   * @param process: process for certain mode dependent startup config.
   * @return vector of command line arguments for application.
   */
  std::vector<std::string> getArgumentsList(const ProcessInfo& process) const;

  /**
   * @brief Method that converts input std::vector of std::string to
   *        std::vector<char*> to pass as argv in application.
   * @param vectorToConvert: vector that will be converted.
   * @return Vector of char* including `nullptr` that be passed to application.
   */
  std::vector<char*>
  convertToNullTerminatingArgv(
      std::vector<std::string> &vectorToConvert);

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
   * @brief Hardcoded path to folder with adaptive applications.
   */
  const static std::string corePath;

  /**
   * @brief structure that holds application and required processes.
   */
  std::map<ProcName, pid_t> m_activeApplications;

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
