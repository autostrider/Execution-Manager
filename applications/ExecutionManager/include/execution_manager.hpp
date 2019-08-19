#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "manifests.hpp"

#include <vector>
#include <string>
#include <cstdint>
#include <unistd.h>
#include <map>

namespace ExecutionManager
{

using MachineState = std::string;
using std::pair;

struct ApplicationManifest;

class ExecutionManager
{
public:
  ExecutionManager() = default;

  /**
     * @brief Main method of Execution manager.
  */
  int start();
private:
  /**
     * Struct for process name and application it belongs to.
  */
  struct ProcessName
  {
    std::string applicationName;
    std::string processName;
  };

  /**
     * @brief Loads all adaptive applications from corePath.
     *
     * @param fileNames: output parameter, where all the applications
     *                   names are stored.
  */
  std::vector<std::string> loadListOfApplications();

  /**
     * @brief processManifests - loads manifests from corePath.
     *
     * @return vector of Application manifest for available
     *          applications respectively.
  */
  void processManifests();
  /**
     * @brief Starts given application and stores information
     *        about it in activeApplications.
     * @param manifest: Application manifest of application to start.
  */
  void startApplication(const ProcessName &manifest);

  /**
     * @brief starts all application that support current state.
  */
  void startApplicationsForState();

  /**
     * @brief kill all processes that doesn't support current state.
  */
  void killProcessesForState();

private:
  /// \brief Hardcoded path to folder with adaptive applications.
  const static std::string corePath;

  /// \brief Structure that holds application and required processes.
  std::map<MachineState, pid_t> activeApplications;

  /// \brief Structure for application that can run in certain state
  /// vector consists of applicationId (name) and string param - executable name.
  std::map<MachineState, std::vector<ProcessName>> allowedApplicationForState;

  /// \brief Current machine state.
  MachineState currentState = "init";

  /// \brief Vector that holds state transitions.
  const static std::vector<MachineState> transition;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
