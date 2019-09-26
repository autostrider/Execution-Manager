#ifndef MANIFESTS_HPP
#define MANIFESTS_HPP


#include <string>
#include <vector>
#include <json.hpp>
#include <application_state_client.h>


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

  std::string createRelativePath() const
  {
    return applicationName
        + "/processes/"
        + processName;
  }

  bool operator==(const ProcessInfo& rhs) const
  {
    return processName == rhs.processName &&
           applicationName == rhs.applicationName;

  }
};

struct ModeDepStartupConfig
{
    std::vector<MachineInstanceMode> modes;

    std::vector<StartupOption> startupOptions;
};

/**
 * @brief The Process struct
 */
struct Process
{
  /// Name of executable.
  std::string name;

  /// Startup configs.
  std::vector<ModeDepStartupConfig> modeDependentStartupConf;
};

struct ApplicationManifestInternal
{
  /** 
   * @brief Name of Adaptive Application.
   */
  std::string manifestId;

  /**
   * @brief Vector of executables of application
   */
  std::vector<Process> processes;
};

/**
 * @brief Application manifest structure.
 */
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

/**
 * @brief Process serialization
 */
void to_json(json& jsonObject, const Process& process);

/**
 * @brief Process deserialization
 */
void from_json(const json& jsonObject, Process& process);

void to_json(json& jsonObject, const ApplicationManifestInternal& manifest);

void from_json(const json& jsonObject, ApplicationManifestInternal& manifest);

/**
 * @brief ApplicationManifest serialization
 */
void to_json(json& jsonObject, const ApplicationManifest& applicationManifest);

/**
 * @brief ApplicationManifest deserialization
 */
void from_json(const json& jsonObject, ApplicationManifest& applicationManifest);

} // namespace ExecutionManager

#endif // MANIFESTS_HPP
