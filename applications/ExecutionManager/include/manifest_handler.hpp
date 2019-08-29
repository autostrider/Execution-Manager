#ifndef MANIFEST_HANDLER_HPP
#define MANIFEST_HANDLER_HPP

#include "manifests.hpp"

#include <string>
#include <map>
#include <vector>

namespace ExecutionManager
{

using MachineState = std::string;
/**
 * @brief Class, responsible for reading manifests from filesystem and
 * processing manifest data.
 */
class ManifestHandler
{
public:
  /**
   * @brief Init fields with data processed from manifest.
   * @param availApps: inout param. Apps that can be in state "running"
   *                   in certain state
   * @param availMachineStates: inout param. Machine states supported by
   *                            machine.
   */
  virtual void
  processManifests(std::map<MachineState, std::vector<ProcessName>>& availApps,
               std::vector<MachineState>& availMachineStates);

  virtual ~ManifestHandler() {}
private:
  /**
    * @brief Load data from application manifests and process it.
    * @return Map of applications for each state in Application
    *         manifest.
    */
   virtual std::map<MachineState, std::vector<ProcessName>>
   processApplicationManifests();

   /**
   * @brief Loading data from Machine Manifest and process it.
   * @return All the machine states available.
   */
  virtual std::vector<MachineState> processMachineManifest();

  /**
   * @brief Loads all adaptive applications from corePath.
   * @return Vector containing names of applications that were found in
   *          corePath.
   */
  std::vector<std::string> loadListOfApplications();

  /**
   * @brief Removes unsupported states from availApps
   * @param availApps: inout param. Apps that can be in state "running"
   *                   in certain state
   * @param availMachineStates: Machine states supported by
   *                            machine.
   */
  void
  filterStates(std::map<MachineState, std::vector<ProcessName>> &availApps,
        std::vector<MachineState> &availMachineStates);
private:
  /**
   * @brief Hardcoded path to folder with adaptive applications.
   */
  const static std::string corePath;

  const static std::string machineStateFunctionGroup;
};

} // namespace ExecutionManager

#endif // MANIFEST_HANDLER_HPP
