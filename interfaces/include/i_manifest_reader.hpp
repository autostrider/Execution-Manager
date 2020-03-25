#ifndef IMANIFEST_READER_HPP
#define IMANIFEST_READER_HPP

#include <constants.hpp>
#include <manifests.hpp>

#include <map>
#include <set>

namespace ExecutionManager
{

class IManifestReader
{
public:
  virtual std::map<MachineState, std::set<ProcName>>
  getStatesSupportedByApplication() = 0;

  virtual std::vector<MachineState> getMachineStates() = 0;

  virtual ~IManifestReader() = default;
};

} // namespace ExecutionManager

#endif // IMANIFEST_READER_HPP
