#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <i_execution_manager_client.hpp>
#include <i_manifest_reader.hpp>
#include <i_application_handler.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

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
  SHUTTINGDOWN,
  SUSPEND
};

class ExecutionManager
{
public:
  ExecutionManager(
    std::unique_ptr<IManifestReader> reader,
    std::unique_ptr<IApplicationHandler> applicationHandler,
    std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> client
  );

  void start();

  void reportApplicationState(const std::string &appName, AppState state);

  MachineState getMachineState() const;

  StateError setMachineState(std::string state);

  void suspend();

private:
  void filterStates();

  void startApplication(const ProcName &process);

  void startApplicationsForState();

  void killProcessesForState();

  bool processToBeKilled (const std::string& app,
                          const std::set<ProcName> &);

  void confirmState(StateError status);

  inline void checkAndSendConfirm();

private:
  std::unique_ptr<IApplicationHandler> m_appHandler;

  std::set<ProcName> m_activeProcesses;

  std::map<MachineState, std::set<ProcName>> m_allowedProcessesForState;

  const static MachineState defaultState;

  MachineState m_currentState;

  MachineState m_pendingState;

  std::vector<MachineState> m_machineManifestStates;

  std::unique_ptr<ExecutionManagerClient::IExecutionManagerClient> m_rpcClient;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
