#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "manifest_handler.hpp"
#include "manifests.hpp"

#include <capnp/ez-rpc.h>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <dirent.h>
#include <exception>
#include <execution_management.capnp.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <json.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace ExecutionManager
{

using applicationId = std::string;
using MachineState = std::string;
using std::pair;

struct ApplicationManifest;

class ExecutionManager final: public ExecutionManagement::Server
{
public:
  ExecutionManager(std::unique_ptr<ManifestHandler> handler =
      std::make_unique<ManifestHandler>());

  /**
   * @brief Main method of Execution manager.
   */
  std::int32_t start();
private:
  using ApplicationState = ::ApplicationStateManagement::ApplicationState;
  using StateError = ::MachineStateManagement::StateError;

  /**
   * @brief Loads all adaptive applications from corePath.
   * @return Vector containing names of applications that were found in
   *         corePath.
   */
  std::vector<std::string> loadListOfApplications();

  /**
   * @brief Starts given application and stores information
   *        about it in activeApplications.
   * @param process: Application to start.
   */
  void startApplication(const ProcessName& process);

  /**
   * @brief starts all application that support current state.
   */
  void startApplicationsForState();

  /**
   * @brief kill all processes that doesn't support current state.
   */
  void killProcessesForState();

  bool processToBeKilled (const std::string& app, const std::vector<ExecutionManager::ProcessName>&);
 
  ::kj::Promise<void>
  reportApplicationState(ReportApplicationStateContext context) override;

  ::kj::Promise<void>
  register_(RegisterContext context) override;

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override;

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override;
private:
  std::unique_ptr<ManifestHandler> manifestHandler;
  /** 
   * @brief Hardcoded path to folder with adaptive applications.
   */
  const static std::string corePath;

  const static std::string machineStateFunctionGroup;

  /** 
   * @brief structure that holds application and required processes.
   */
  std::map<MachineState, pid_t> m_activeApplications;

  /**
   * @brief Structure for application that can run in certain state
   * vector consists of applicationId (name) and string param - executable name.
   */
  std::map<MachineState, std::vector<ProcessName>> m_allowedApplicationForState;
  /**
   * brief Current machine state.
   */
  MachineState m_currentState;
  /**
   * @brief Vector that holds state transitions.
   */
  std::vector<MachineState> m_machineManifestStates;

  std::string machineStateClientAppName;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
