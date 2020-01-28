#ifndef MANIFESTS_HPP
#define MANIFESTS_HPP

#include <application_state_management.capnp.h>

#include <string>
#include <vector>
#include <json.hpp>


namespace ExecutionManager
{

using nlohmann::json;
using MachineState = std::string;
using ApplicationState = ::ApplicationStateManagement::ApplicationState;

struct MachineInstanceMode
{
    std::string functionGroup;
    std::string mode;
};

enum class StartupOptionKindEnum
{
  commandLineLongForm,
  commandLineShortForm,
  commandLineSimpleForm
};

struct StartupOption
{
  StartupOptionKindEnum optionKind;
  std::string optionName;
  std::string optionArg;

  std::string makeCommandLineOption() const;
};

struct ProcessInfo
{
  std::string applicationName;
  std::string processName;
  std::vector<StartupOption> startOptions;
};

struct ModeDepStartupConfig
{
    std::vector<MachineInstanceMode> modes;

    std::vector<StartupOption> startupOptions;
};

struct Process
{
  std::string name;

  std::vector<ModeDepStartupConfig> modeDependentStartupConf;
};

struct ApplicationManifestInternal
{
  std::string manifestId;

  std::vector<Process> processes;
};

struct ApplicationManifest
{
    ApplicationManifestInternal manifest;
};

void to_json(json& jsonObject, const MachineInstanceMode& machineInstanceMode);

struct Mode
{
  MachineState mode;
};


struct ModeDeclarationGroup
{
  std::string functionGroupName;

  std::vector<Mode> modeDeclarations;

};

struct MachineManifestInternal
{
  std::string manifestId;

  std::vector<ModeDeclarationGroup> modeDeclarationGroups;
};

struct MachineManifest
{
  MachineManifestInternal manifest;
};


void to_json(json& jsonObject, const StartupOption& startupOptions);

void from_json(const json& jsonObject, StartupOption& startupOptions);

void to_json(json& jsonObject, const Mode& mode);

void from_json(const json& jsonObject, Mode& mode);

void to_json(json& jsonObject, const ModeDeclarationGroup& modeDeclGroup);

void from_json(const json& jsonObject, ModeDeclarationGroup& modeDeclGroup);

void to_json(json& jsonObject, const MachineManifestInternal& manifest);

void from_json(const json& jsonObject, MachineManifestInternal& manifest);

NLOHMANN_JSON_SERIALIZE_ENUM(StartupOptionKindEnum, {
    {StartupOptionKindEnum::commandLineSimpleForm, "commandLineSimpleForm"},
    {StartupOptionKindEnum::commandLineShortForm, "commandLineShortForm"},
    {StartupOptionKindEnum::commandLineLongForm, "commandLineLongForm"}
})


void to_json(json& jsonObject, const MachineManifest& manifest);

void from_json(const json& jsonObject, MachineManifest& manifest);

void to_json(json& jsonObject, const MachineInstanceMode& machineInstanceMode);

void
from_json(const json& jsonObject, MachineInstanceMode& machineInstanceMode);

void to_json(json& jsonObject, const ModeDepStartupConfig& startupConf);

void
from_json(const json& jsonObject, const ModeDepStartupConfig& startupConf);

void to_json(json& jsonObject, const Process& process);

void from_json(const json& jsonObject, Process& process);

void to_json(json& jsonObject, const ApplicationManifestInternal& manifest);

void from_json(const json& jsonObject, ApplicationManifestInternal& manifest);

void to_json(json& jsonObject, const ApplicationManifest& applicationManifest);

void from_json(const json& jsonObject, ApplicationManifest& applicationManifest);

} // namespace ExecutionManager

#endif // MANIFESTS_HPP
