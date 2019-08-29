#ifndef MANIFESTS_HPP
#define MANIFESTS_HPP

#include <sys/unistd.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <json.hpp>
#include <application_state_management.capnp.h>
#include <machine_state_management.capnp.h>
#include <application_state_client.h>


namespace ExecutionManager
{

using nlohmann::json;
using std::ifstream;
using MachineState = std::string;
using ApplicationState = ::ApplicationStateManagement::ApplicationState;

struct MachineInstanceMode
{
    std::string functionGroup;
    std::string mode;
};

enum class StartupOptionKind
{
  commandLineSimpleForm,
  commandLineShortForm,
  commandLineLongForm
};

struct StartupOption
{
  StartupOptionKind optionKind;
  std::string optionName;
  std::string optionArg;

  std::string makeCommandLineOption() const;
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

struct Manifest
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
    Manifest manifest;
};

void to_json(json& jsonObject, const MachineInstanceMode& machineInstanceMode);

void
from_json(const json& jsonObject, MachineInstanceMode& machineInstanceMode);

NLOHMANN_JSON_SERIALIZE_ENUM(StartupOptionKind, {
    {StartupOptionKind::commandLineSimpleForm, "commandLineSimpleForm"},
    {StartupOptionKind::commandLineShortForm, "commandLineShortForm"},
    {StartupOptionKind::commandLineLongForm, "commandLineLongForm"}
})

void to_json(json& jsonObject, const StartupOption& startupOptions);

void from_json(const json& jsonObject, StartupOption& startupOptions);

void to_json(json& jsonObject, const ModeDepStartupConfig& startupConf);

void
from_json(const json& jsonObject, const ModeDepStartupConfig& startupConf);

/**
 * @brief Process serialization
 */
void to_json(json& jsonObject, const ModeDepStartupConfig& startupConf);

/**
 * @brief Process deserialization
 */
void from_json(const json& jsonObject, Process& process);

void to_json(json& jsonObject, const Manifest& manifest);

void from_json(const json& jsonObject, Manifest& manifest);

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
