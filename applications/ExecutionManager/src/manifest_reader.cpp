#include "manifest_reader.hpp"
#include <common.hpp>

#include <dirent.h>
#include <json.hpp>
#include <fstream>
#include <algorithm>

namespace ExecutionManager {

using std::runtime_error;
using nlohmann::json;
using std::ifstream;

const std::string ManifestReader::machineStateFunctionGroup = "MachineState";

json ManifestReader::getJsonData(const std::string& manifestPath)
{
  ifstream data{manifestPath};
  json manifestData;
  data >> manifestData;

  return manifestData;
}

ManifestReader::ManifestReader(const ManifestReaderConf &conf)
  : conf(conf)
{ }

std::map<MachineState, ProcessesList> ManifestReader::getStatesSupportedByApplication()
{
  const auto& applicationNames = getListOfApplications();
  std::map<MachineState, ProcessesList> res;
  const static std::string manifestFile = "/manifest.json";

  for (auto file: applicationNames)
  {
    file = conf.pathToApplicationsFolder + "/" + file + manifestFile;

    json content = getJsonData(file);

    ApplicationManifest manifest = content.get<ApplicationManifest>();

    for (const auto& process: manifest.manifest.processes)
    {
      for (const auto& conf: process.modeDependentStartupConf)
      {
        for (const auto& mode: conf.modes)
        {
          if (mode.functionGroup == machineStateFunctionGroup)
          {
            res[mode.mode]
              .insert(getServiceName(manifest.manifest.manifestId, process.name));
          }
        }
      }
    }
  }

  return res;
}

std::vector<MachineState> ManifestReader::getMachineStates()
{
  json manifestData = getJsonData(conf.machineManifestFilePath);
  MachineManifest manifest = manifestData.get<MachineManifest>();

  const auto& availableMachineStates =
      std::find_if(manifest.manifest.modeDeclarationGroups.cbegin(),
                   manifest.manifest.modeDeclarationGroups.cend(),
                   [=](const ModeDeclarationGroup& group)
  { return group.functionGroupName == machineStateFunctionGroup; });

  // check whether this function group existed in manifest
  if (manifest.manifest.modeDeclarationGroups.cend() == availableMachineStates)
  {
    return {};
  }

  std::vector<MachineState> res;
  res.reserve(availableMachineStates->modeDeclarations.size());
  std::transform(availableMachineStates->modeDeclarations.cbegin(),
                 availableMachineStates->modeDeclarations.cend(),
                 std::back_inserter(res),
                 [](const Mode& mode) {return mode.mode;});

  return res;
}

ProcessesList ManifestReader::getListOfApplications()
{
  DIR* dp = nullptr;
  ProcessesList fileNames;

  if ((dp = opendir(conf.pathToApplicationsFolder.c_str())) == nullptr)
  {
    throw runtime_error(std::string{"Error opening directory: "}
                        + conf.pathToApplicationsFolder
                        + " "
                        + strerror(errno));
  }

  for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
  {
    if (drnt->d_name != std::string{"."} &&
        drnt->d_name != std::string{".."})
    {
      fileNames.emplace(drnt->d_name);
    }
  }

  closedir(dp);
  return fileNames;
}

} // namespace ExecutionManager
