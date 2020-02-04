#ifndef MANIFEST_HANDLER_HPP
#define MANIFEST_HANDLER_HPP

#include <i_manifest_reader.hpp>

namespace ExecutionManager
{

using MachineState = std::string;

struct ManifestReaderConf
{
  const std::string pathToApplicationsFolder{"./bin/applications"};
  const std::string machineManifestFilePath{"../applications/ExecutionManager/machine_manifest.json"};
};

class ManifestReader : public IManifestReader
{
public:
  explicit ManifestReader(const ManifestReaderConf& conf = ManifestReaderConf{});

   std::map<MachineState, std::set<ProcName>>
   getStatesSupportedByApplication() override;

  std::vector<MachineState> getMachineStates() override;


  ~ManifestReader() override = default;

private:
  json getJsonData(const std::string &manifestPath);

  std::set<ProcName> getListOfApplications();

  const ManifestReaderConf conf;

  const static std::string machineStateFunctionGroup;
};

} // namespace ExecutionManager

#endif // MANIFEST_HANDLER_HPP
