#ifndef JSONCONVERTERS_HPP
#define JSONCONVERTERS_HPP

#include "../json/json.hpp"
#include "manifests.hpp"

using nlohmann::json;

namespace manifests 
{

// MachineStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(MachineStates, {
    {MachineStates::kInit, "init"},
    {MachineStates::kRestart, "restart"},
    {MachineStates::kRunning, "running"},
    {MachineStates::kShutdown, "shutdown"}
})
// —————————————————————————————————————————

// AppStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(AppStates, {
    {AppStates::kInitializing, "init"},
    {AppStates::kRunning, "running"},
    {AppStates::kShuttingdown, "shutdown"}
})
// —————————————————————————————————————————

// InterfaceConf serialization & deserialization
void to_json(json& j, const InterfaceConf& interfaceConf) 
{
    j = json{
            {"ifa_name", interfaceConf.ifa_name},
            {"family", interfaceConf.family},
            {"host", interfaceConf.host}
    };
}

void from_json(const json& j, InterfaceConf& interfaceConf) 
{
    j.at("ifa_name").get_to(interfaceConf.ifa_name);
    j.at("family").get_to(interfaceConf.family);
    j.at("host").get_to(interfaceConf.host);
}
// —————————————————————————————————————————

// HwConf serialization & deserialization
void to_json(json& j, const HwConf& hwConf) 
{
    j = json{
            {"ram", hwConf.ram},
            {"cpu", hwConf.cpu}
    };
}


void from_json(const json& j, HwConf& hwConf) 
{
    j.at("ram").get_to(hwConf.ram);
    j.at("cpu").get_to(hwConf.cpu);
}
// —————————————————————————————————————————

// MachineState serialization & deserialization
void to_json(json& j, const MachineManifest& machineManifest) 
{
    j = json{
            {"network", machineManifest.network},
            {"hwConf", machineManifest.hwConf},
            {"states", machineManifest.states},
            {"adaptiveModules", machineManifest.adaptiveModules}
    };
}

void from_json(const json& j, MachineManifest& machineManifest) 
{
    j.at("network").get_to(machineManifest.network);
    j.at("hwConf").get_to(machineManifest.hwConf);
    j.at("states").get_to(machineManifest.states);
    j.at("adaptiveModules").get_to(machineManifest.adaptiveModules);
}
// —————————————————————————————————————————

// Startup serialization & deserialization
void to_json(json& j, const StartupDeps& startupDeps) 
{
    j = json{
            {"name", startupDeps.name},
            {"reqState", startupDeps.reqState}
    };
}

void from_json(const json& j, StartupDeps& startupDeps) 
{
    j.at("name").get_to(startupDeps.name);
    j.at("reqState").get_to(startupDeps.reqState);
}
// —————————————————————————————————————————

// ApplicationManifest serialization & deserialization
void to_json(json& j, const ApplicationManifest& applicationManifest) 
{
    j = json{
            {"name", applicationManifest.name},
            {"version", applicationManifest.version},
            {"machineStates", applicationManifest.machineStates},
            {"deps", applicationManifest.deps}
    };
}

void from_json(const json& j, ApplicationManifest& applicationManifest) 
{
    j.at("name").get_to(applicationManifest.name);
    j.at("version").get_to(applicationManifest.version);
    j.at("machineStates").get_to(applicationManifest.machineStates);
    j.at("deps").get_to(applicationManifest.deps);
}
// —————————————————————————————————————————
}

#endif // JSONCONVERTERS_HPP