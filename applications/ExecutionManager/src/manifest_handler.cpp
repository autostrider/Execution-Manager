#include "manifest_handler.hpp"

#include <dirent.h>
#include <exception>
#include <json.hpp>
#include <algorithm>

namespace ExecutionManager {

using std::runtime_error;
using nlohmann::json;

const std::string ManifestReader::corePath = "./bin/applications/";

const std::string ManifestReader::machineStateFunctionGroup = "MachineState";


void ManifestReader::filterStates(
    std::map<MachineState, std::vector<ProcessName>> &availApps,
    std::vector<MachineState> &availMachineStates)
{
  for (auto app = availApps.begin(); app != availApps.end();)
  {
    if (std::find(availMachineStates.cbegin(),
                  availMachineStates.cend(),
                  app->first) == availMachineStates.cend())
    {
      app = availApps.erase(app);
    }
    else
    {
      app++;
    }
  }
}

std::map<MachineState, std::vector<ProcessName>>
ManifestReader::processApplicationManifests()
{
  const auto& applicationNames = loadListOfApplications();
  std::map<MachineState, std::vector<ProcessName>> res;

  for (auto file: applicationNames)
  {
    file = corePath + file + "/manifest.json";

    ifstream data{file};
    json content;
    data >> content;

    ApplicationManifest manifest = content.get<ApplicationManifest>();

    for (const auto& process: manifest.manifest.processes)
    {
      for (const auto& conf: process.modeDependentStartupConf)
      {
        for (const auto& mode: conf.modes)
        {
          if (mode.functionGroup != machineStateFunctionGroup)
          {
            continue;
          }

          res[mode.mode]
              .push_back({manifest.manifest.manifestId, process.name});
        }
      }
    }
  }

  return res;
}

std::vector<MachineState> ManifestReader::processMachineManifest()
{
  static const std::string manifestPath =
      "../applications/ExecutionManager/machine_manifest.json";

  ifstream data{manifestPath};
  json manifestData;

  data >> manifestData;
  MachineManifest manifest = manifestData.get<MachineManifest>();

  std::vector<MachineState> res;
  const auto& availableMachineStates =
      std::find_if(manifest.manifest.modeDeclarationGroups.cbegin(),
                   manifest.manifest.modeDeclarationGroups.cend(),
                   [=](const ModeDeclarationGroup& group)
  { return group.functionGroupName == machineStateFunctionGroup; });

  std::transform(availableMachineStates->modeDeclarations.cbegin(),
                 availableMachineStates->modeDeclarations.cend(),
                 std::back_inserter(res),
                 [](const Mode& mode) {return mode.mode;});

  return res;
}

std::vector<std::string> ManifestReader::loadListOfApplications()
{
  DIR* dp = nullptr;
  std::vector<std::string> fileNames;

  if ((dp = opendir(corePath.c_str())) == nullptr)
  {
    throw runtime_error(std::string{"Error opening directory: "}
                        + corePath
                        + " "
                        + strerror(errno));
  }

  for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
  {
    if (drnt->d_name == std::string{"."} ||
        drnt->d_name == std::string{".."})
    {
      continue;
    }

    fileNames.emplace_back(drnt->d_name);

    std::cout << drnt->d_name << std::endl;
  }

  closedir(dp);
  return fileNames;
}

} // namespace ExecutionManager
