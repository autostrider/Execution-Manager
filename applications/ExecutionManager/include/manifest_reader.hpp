#ifndef MANIFEST_HANDLER_HPP
#define MANIFEST_HANDLER_HPP

#include "manifests.hpp"
#include "imanifest_reader.hpp"

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
class ManifestReader : public IManifestReader
{
public:
  /**
    * @brief Load data from application manifests and process it.
    * @return Map of applications for each state in Application
    *         manifest.
    */
   virtual std::map<MachineState, std::vector<ProcessName>>
   getStatesSupportedByApplication() override;

   /**
   * @brief Loading data from Machine Manifest and process it.
   * @return All the machine states available.
   */
  virtual std::vector<MachineState> getMachineStates() override;

  /**
   * @brief Load json from file.
   * @param manifestPath: path to file.
   * @return parsed object.
   */
  json getJsonData(const std::string &manifestPath);

  ~ManifestReader() override = default;

#ifndef UNIT_TEST
private:
#endif

  /**
   * @brief Loads all adaptive applications from corePath.
   * @return Vector containing names of applications that were found in
   *          corePath.
   */
  std::vector<std::string> getListOfApplications();

  /**
   * @brief Hardcoded path to folder with adaptive applications.
   */
  const std::string corePath =
      #ifndef UNIT_TEST
        "./bin/applications/";
      #else
        "./test-data/";
      #endif

  const static std::string machineStateFunctionGroup;
};

} // namespace ExecutionManager

#endif // MANIFEST_HANDLER_HPP
