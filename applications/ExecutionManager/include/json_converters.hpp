#ifndef JSON_CONVERTERS_HPP
#define JSON_CONVERTERS_HPP

#include <json.hpp>
#include <application_state_client.h>
#include "manifests.hpp"

using nlohmann::json;

namespace ExecutionManager
{

// MachineStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(MachineStates, {
    {MachineStates::kInit, "init"},
    {MachineStates::kRestart, "restart"},
    {MachineStates::kRunning, "running"},
    {MachineStates::kShutdown, "shutdown"}
})


// AppStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(ApplicationState, {
    {ApplicationState::K_INITIALIZING, "init"},
    {ApplicationState::K_RUNNING, "running"},
    {ApplicationState::K_SHUTTINGDOWN, "shutdown"}
})


/// InterfaceConf serialization
void to_json(json& j, const InterfaceConf& interfaceConf)
{
    j = json{
            {"ifa_name", interfaceConf.ifa_name},
            {"family", interfaceConf.family},
            {"host", interfaceConf.host}
    };
}

/// InterfaceConf  deserialization
void from_json(const json& j, InterfaceConf& interfaceConf)
{
    j.at("ifa_name").get_to(interfaceConf.ifa_name);
    j.at("family").get_to(interfaceConf.family);
    j.at("host").get_to(interfaceConf.host);
}

/// HwConf serialization
void to_json(json& j, const HwConf& hwConf)
{
    j = json{
            {"ram", hwConf.ram},
            {"cpu", hwConf.cpu}
    };
}

/// HwConf  deserialization
void from_json(const json& j, HwConf& hwConf)
{
    j.at("ram").get_to(hwConf.ram);
    j.at("cpu").get_to(hwConf.cpu);
}

/// MachineState serialization
void to_json(json& j, const MachineManifest& machineManifest)
{
    j = json{
            {"network", machineManifest.network},
            {"hwConf", machineManifest.hwConf},
            {"states", machineManifest.states},
            {"adaptiveModules", machineManifest.adaptiveModules}
    };
}

/// MachineManifest deserialization
void from_json(const json& j, MachineManifest& machineManifest)
{
    j.at("network").get_to(machineManifest.network);
    j.at("hwConf").get_to(machineManifest.hwConf);
    j.at("states").get_to(machineManifest.states);
    j.at("adaptiveModules").get_to(machineManifest.adaptiveModules);
}

/// Process serialization
void to_json(json& j, const Process& process)
{
    j = json{
          {"process_name", process.name},
          {"avail_start_machine_states", process.startMachineStates}
    };
}

/// Process deserialization
void from_json(const json& j, Process& process)
{
    j.at("process_name").get_to(process.name);
    j.at("avail_start_machine_states").get_to(process.startMachineStates);
}


/// ApplicationManifest serialization
void to_json(json& j, const ApplicationManifest& applicationManifest)
{
    j = json{
            {"name", applicationManifest.name},
            {"processes", applicationManifest.processes}
    };
}

/// ApplicationManifest deserialization
void from_json(const json& j, ApplicationManifest& applicationManifest)
{
    j.at("name").get_to(applicationManifest.name);
    j.at("processes").get_to(applicationManifest.processes);
}

}

#endif // JSON_CONVERTERS_HPP
