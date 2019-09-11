#ifndef MANIFEST_HANDLER_HPP
#define MANIFEST_HANDLER_HPP

#include "manifests.hpp"
#include "imanifest_reader.hpp"

#include <string>

namespace ExecutionManager
{

using MachineState = std::string;

struct ManifestReaderConf
{
  const std::string pathToApplicationsFolder{"./bin/applications"};
  const std::string machineManifestFilePath{ "../applications/ExecutionManager/machine_manifest.json"};
};

/**
 * @brief Class, responsible for reading manifests from filesystem and
 * processing manifest data.
 */
class ManifestReader : public IManifestReader
{
public:
  ManifestReader(const ManifestReaderConf& conf = ManifestReaderConf{});
  /**
    * @brief Load data from application manifests and process it.
    * @return Map of applications for each state in Application
    *         manifest.
    */
   virtual std::map<MachineState, std::vector<ProcessInfo>>
   getStatesSupportedByApplication() override;

   /**
   * @brief Loading data from Machine Manifest and process it.
   * @return All the machine states available.
   */
  virtual std::vector<MachineState> getMachineStates() override;


  ~ManifestReader() override = default;

private:
  /**
   * @brief Load json from file.
   * @param manifestPath: path to file.
   * @return parsed object.
   */
  json getJsonData(const std::string &manifestPath);

  /**
   * @brief Loads all adaptive applications from corePath.
   * @return Vector containing names of applications that were found in
   *          corePath.
   */
  std::vector<std::string> getListOfApplications();

  /**
   * @brief Config with pathes to machine manfiest and
   *        folder with applications;
   */
  const ManifestReaderConf conf;

  const static std::string machineStateFunctionGroup;
};

} // namespace ExecutionManager

#endif // MANIFEST_HANDLER_HPP
