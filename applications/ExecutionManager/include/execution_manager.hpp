#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <csignal>
#include <cstdint>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <iostream>
#include <functional>
#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>
#include "manifests.hpp"

namespace ExecutionManager
{

using applicationId = std::string;

struct ApplicationManifest;

class ExecutionManager final: public ExecutionManagement::Server
{
public:
  /**
    * @brief Main method of Execution manager.
    */
  std::int32_t start();
private:
  using ApplicationState = ::ApplicationStateManagement::ApplicationState;
  using StateError = ::MachineStateManagement::StateError;

  /**
   * @brief Loads all adaptive applications from corePath.
   * @param fileNames: output parameter, where all the applications
   *                   names are stored.
   */
  void loadListOfApplications(std::vector<std::string>& fileNames);

  /**
   * @brief processManifests - loads manifests from corePath.
   * @return vector of Application manifest for available
   *          applications respectively.
   */
  std::vector<ApplicationManifest> processManifests();
  /**
   * @brief Starts given application and stores information
   *        about it in activeApplications.
   * @param manifest: Application manifest of application to start.
   */
  void startApplication(const ApplicationManifest &manifest);

  ::kj::Promise<void>
  reportApplicationState(ReportApplicationStateContext context) override;

  ::kj::Promise<void>
  register_(RegisterContext context) override;

  ::kj::Promise<void>
  getMachineState(GetMachineStateContext context) override;

  ::kj::Promise<void>
  setMachineState(SetMachineStateContext context) override;
private:

  /// \brief Hardcoded path to folder with adaptive applications.
  const static std::string corePath;

   /// \brief structure that holds application and required processes.
  std::map<applicationId, std::vector<pid_t>> activeApplications;

  std::string machineState{"kRunning"};

  std::string machineStateClientAppName;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
