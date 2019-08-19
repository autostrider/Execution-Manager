#ifndef MANIFESTS_HPP
#define MANIFESTS_HPP

#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/unistd.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <json.hpp>

#include <application_state_client.h>

using nlohmann::json;

namespace ExecutionManager
{

using std::ifstream;

using api::ApplicationState;

/**
 * @brief Struct, that holds network interface configuration.
 *
 * @field ifa_name: name of interface
 * @field family: family of interface
 */
struct InterfaceConf
{
  std::string ifa_name;
  std::string family;
  char host[NI_MAXHOST];
};

/**
 * @brief Struct that holds current hardware configuration.
 *
 * @field ram: current available ram for running Adaptive Platform
 * @field cpu: available cpu power for running Adaptive Platform
 */
struct HwConf
{
  uint64_t ram;
  uint8_t cpu;
};

/**
 * @brief Struct that holds machine manifest configuration.
 *
 * network: vectors that holds all network interfaces
 * @field hwConf: hardware configuration for running Adaptive Platform
 * @field states: available machine states
 * @field adaptiveModules: all the available Adaptive Modules
 */
struct MachineManifest
{
 public:
  std::vector<InterfaceConf> network;
  HwConf hwConf;
  std::vector<MachineStates> states;
  std::vector<std::string> adaptiveModules;

 public:
 /**
  * @brief Method that generates machine manifest.
  */
  void init();

 private:
  /**
   * @brief Method that loads available network interfaces and its' configs.
   */
  void loadNetworkConf();

  /**
   * @brief Method that loads current hardware configuration
   */
  void loadHwConf();
};

struct MachineInstanceMode
{
    std::string functionGroup;
    std::string mode;
};

struct ModeDepStartupConfig
{
    std::vector<MachineInstanceMode> modes;
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
    /// Name of Adaptive Application.
    std::string manifestId;
    /// Vector of executables of application
    std::vector<Process> processes;
};

/**
 * @brief Application manifest structure.
 */
struct ApplicationManifest
{
    Manifest manifest;
};


/// AppStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(api::ApplicationState, {
    {api::ApplicationState::kInitializing, "init"},
    {api::ApplicationState::kRunning, "running"},
    {api::ApplicationState::kShuttingdown, "shutdown"}
})

/// InterfaceConf serialization
void to_json(json& jsonObject, const InterfaceConf& interfaceConf);

/// InterfaceConf  deserialization
void from_json(const json& jsonObject, InterfaceConf& interfaceConf);

/// HwConf serialization
void to_json(json& jsonObject, const HwConf& hwConf);

/// HwConf  deserialization
void from_json(const json& jsonObject, HwConf& hwConf);

/// MachineState serialization
void to_json(json& jsonObject, const MachineManifest& machineManifest);

/// MachineManifest deserialization
void from_json(const json& jsonObject, MachineManifest& machineManifest);

void to_json(json& jsonObject, const MachineInstanceMode& machineInstanceMode);

void
from_json(const json& jsonObject, MachineInstanceMode& machineInstanceMode);

void to_json(json& jsonObject, const ModeDepStartupConfig& startupConf);

void
from_json(const json& jsonObject, const ModeDepStartupConfig& startupConf);

/// Process serialization
void to_json(json& jsonObject, const ModeDepStartupConfig &startupConf);

/// Process deserialization
void from_json(const json& jsonObject, Process& process);

void to_json(json& jsonObject, const Manifest& manifest);

void from_json(const json& jsonObject, Manifest& manifest);

/// ApplicationManifest serialization
void to_json(json& jsonObject, const ApplicationManifest& applicationManifest);

/// ApplicationManifest deserialization
void from_json(const json& jsonObject, ApplicationManifest& applicationManifest);

} // namespace ExecutionManager

#endif // MANIFESTS_HPP
