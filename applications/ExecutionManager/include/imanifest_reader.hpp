#ifndef IMANIFEST_READER_HPP
#define IMANIFEST_READER_HPP

#include "manifests.hpp"

#include <map>
#include <vector>

namespace ExecutionManager {

class IManifestReader
{
public:
  virtual std::map<MachineState, std::vector<ProcessName>>
  processApplicationManifests() = 0;

  virtual std::vector<MachineState> processMachineManifest() = 0;

  virtual ~IManifestReader() {}
};

} // namespace ExecutionManager

#endif // IMANIFEST_READER_HPP
