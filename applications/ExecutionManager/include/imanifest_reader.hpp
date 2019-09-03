#ifndef IMANIFEST_READER_HPP
#define IMANIFEST_READER_HPP

#include "manifests.hpp"

#include <map>
#include <vector>

namespace ExecutionManager
{

/**
 * @brief Manifest processing interfaface. It provides methods for
 *        reading manifests data.
 */
class IManifestReader
{
public:
  /**
    * @brief Load data from application manifests and process it.
    * @return Map of applications for each state in Application
    *         manifest.
    */
  virtual std::map<MachineState, std::vector<ProcessName>>
  getApplicationStatesMap() = 0;

  /**
  * @brief Loading data from Machine Manifest and process it.
  * @return All the machine states available.
  */
  virtual std::vector<MachineState> getMachineStates() = 0;

  virtual ~IManifestReader() = default;
};

} // namespace ExecutionManager

#endif // IMANIFEST_READER_HPP
