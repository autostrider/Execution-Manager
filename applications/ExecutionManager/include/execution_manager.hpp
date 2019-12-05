#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <i_execution_manager_client.hpp>
#include <i_manifest_reader.hpp>
#include <i_application_handler.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <set>

namespace ExecutionManager
{

using MachineState = std::string;
using ComponentState = std::string;
using ProcName = std::string;
using StateError = ::MachineStateManagement::StateError;
using ComponentClientReturnType = ::StateManagement::ComponentClientReturnType;
using StateUpdateMode = ::StateManagement::StateUpdateMode;
using std::pair;

struct ApplicationManifest;

enum class AppState : uint16_t
{
  kInitializing,
  kRunning,
  kShuttingDown
};

class ExecutionManager
{
public:
  ExecutionManager(std::unique_ptr<IManifestReader> reader,
                   std::unique_ptr<IApplicationHandler> applicationHandler,
                   std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client);


  void start();

  void reportApplicationState(const std::string& appName, AppState state);

  MachineState getMachineState() const;

  StateError setMachineState(std::string state);

  void registerComponent(std::string component, StateUpdateMode updateMode);

  ComponentClientReturnType
  getComponentState(std::string component, ComponentState& state) const;

  void confirmComponentState
  (std::string component, ComponentState state, ComponentClientReturnType status);

private:
  void filterStates();

  void startApplication(const ProcName &process);

  void startApplicationsForState();

  void killProcessesForState();

  bool processToBeKilled (const std::string& app,
                          const std::set<ProcName> &);

  void confirmState(StateError status);

  inline void checkAndSendConfirm();

  inline bool isConfirmAvailable();

  void changeComponentsState();

private:
  std::unique_ptr<IApplicationHandler> m_appHandler;

  std::set<ProcName> m_activeProcesses;

  std::map<MachineState, std::set<ProcName>> m_allowedProcessesForState;

  const static MachineState defaultState;

  MachineState m_currentState;

  MachineState m_pendingState;

  ComponentState m_currentComponentState;

  std::vector<MachineState> m_machineManifestStates;

  std::set<pid_t> m_stateConfirmToBeReceived;

  std::set<std::string> m_registeredComponents;
  std::set<std::string> m_componentPendingConfirms;
  std::vector<std::string> m_componentStateUpdateSbscrs;

  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> m_rpcClient;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
