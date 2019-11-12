#ifndef IMANIFEST_READER_HPP
#define IMANIFEST_READER_HPP

#include "manifests.hpp"

#include <map>
#include <vector>
#include <set>

namespace ExecutionManager
{

class IManifestReader
{
public:
  virtual std::map<MachineState, std::set<std::string>>
  getStatesSupportedByApplication() = 0;

  virtual std::vector<MachineState> getMachineStates() = 0;

  virtual ~IManifestReader() = default;
};

} // namespace ExecutionManager

#endif // IMANIFEST_READER_HPP
