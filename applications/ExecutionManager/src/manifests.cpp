#include "manifests.hpp"

#include <json.hpp>
#include <application_state_client.h>

using nlohmann::json;

namespace ExecutionManager
{

void to_json(json& jsonObject, const MachineInstanceMode& machineInstanceMode)
{
  jsonObject = json{
    {"Function_group", machineInstanceMode.functionGroup},
    {"Mode", machineInstanceMode.mode}
  };
}

void from_json(const json& jsonObject, MachineInstanceMode& machineInstanceMode)
{
  jsonObject.at("Function_group").get_to(machineInstanceMode.functionGroup);
  jsonObject.at("Mode").get_to(machineInstanceMode.mode);
}

void to_json(json& jsonObject, const ModeDepStartupConfig& startupConf)
{
  jsonObject = json{
    {"Mode_in_machine_instance_refs", startupConf.modes}
  };
}

void from_json(const json& jsonObject, ModeDepStartupConfig& startupConf)
{
  jsonObject.at("Mode_in_machine_instance_refs").get_to(startupConf.modes);
}

// Process serialization
void to_json(json& jsonObject, const Process& process)
{
  jsonObject = json{
      {"Process_name", process.name},
      {"Mode_dependent_startup_configs", process.modeDependentStartupConf}};
}

// Process deserialization
void from_json(const json& jsonObject, Process& process)
{
  jsonObject.at("Process_name").get_to(process.name);
  jsonObject.at("Mode_dependent_startup_configs")
    .get_to(process.modeDependentStartupConf);
}

void to_json(json& jsonObject, const ApplicationManifestInternal& manifest)
{
  jsonObject = json{
    {"Application_manifest_id", manifest.manifestId},
    {"Process", manifest.processes}
  };
}

void from_json(const json& jsonObject, ApplicationManifestInternal& manifest)
{
  jsonObject.at("Application_manifest_id").get_to(manifest.manifestId);
  jsonObject.at("Process").get_to(manifest.processes);
}

// ApplicationManifest serialization
void to_json(json& jsonObject, const ApplicationManifest& applicationManifest)
{
  jsonObject = json{ {"Application_manifest", applicationManifest.manifest } };
}

// ApplicationManifest deserialization
void from_json(const json& jsonObject, ApplicationManifest& applicationManifest)
{
  jsonObject.at("Application_manifest").get_to(applicationManifest.manifest);
}

void to_json(json &jsonObject, const Mode &mode)
{
  jsonObject = json{
    {"Mode", mode.mode}
};
}

void from_json(const json &jsonObject, Mode &mode)
{
  jsonObject.at("Mode").get_to(mode.mode);
}

void to_json(json &jsonObject, const ModeDeclarationGroup& modeDeclGroup)
{
  jsonObject = json{
    {"Function_group_name", modeDeclGroup.functionGroupName},
    {"Mode_declarations", modeDeclGroup.modeDeclarations}
};
}

void from_json(const json &jsonObject, ModeDeclarationGroup &modeDeclGroup)
{
  jsonObject.at("Function_group_name").get_to(modeDeclGroup.functionGroupName);
  jsonObject.at("Mode_declarations").get_to(modeDeclGroup.modeDeclarations);
}

void to_json(json &jsonObject, const MachineManifestInternal &manifest)
{
  jsonObject = json{
    {"Machine_manifest_id", manifest.manifestId},
    {"Mode_declaration_group", manifest.modeDeclarationGroups}
};
}

void from_json(const json &jsonObject, MachineManifestInternal &manifest)
{
  jsonObject.at("Machine_manifest_id").get_to(manifest.manifestId);
  jsonObject.at("Mode_declaration_group").get_to(manifest.modeDeclarationGroups);
}

void to_json(json &jsonObject, const MachineManifest &manifest)
{
  jsonObject = json{
    {"Machine_manifest", manifest.manifest}
};
}

void from_json(const json &jsonObject, MachineManifest &manifest)
{
  jsonObject.at("Machine_manifest").get_to(manifest.manifest);
}

} // namespace ExecutionManager
