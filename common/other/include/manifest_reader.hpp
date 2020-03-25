#ifndef MANIFEST_HANDLER_HPP
#define MANIFEST_HANDLER_HPP

#include <i_manifest_reader.hpp>

#include <json.hpp>

namespace common
{

using MachineState = std::string;
using nlohmann::json;

struct ManifestReaderConf
{
  const std::string pathToApplicationsFolder{"./bin/applications"};
  const std::string machineManifestFilePath{"../applications/ExecutionManager/machine_manifest.json"};
};

class ManifestReader : public api::IManifestReader
{
public:
  explicit ManifestReader(const ManifestReaderConf& conf = ManifestReaderConf{});

   std::map<MachineState, std::set<std::string>>
   getStatesSupportedByApplication() override;

  std::vector<MachineState> getMachineStates() override;


  ~ManifestReader() override = default;

private:
  json getJsonData(const std::string &manifestPath);

  std::set<std::string> getListOfApplications();

  const ManifestReaderConf conf;

  const static std::string machineStateFunctionGroup;
};

} // namespace common

#endif // MANIFEST_HANDLER_HPP
