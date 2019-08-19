#include "manifests.hpp"

#include <json.hpp>
#include <application_state_client.h>

using nlohmann::json;

namespace ExecutionManager
{

void MachineManifest::init()
{
  loadHwConf();
  loadNetworkConf();
  states = {"init"};
  adaptiveModules = {};
}

void MachineManifest::loadNetworkConf()
{
  ifaddrs *ifa = nullptr;
  getifaddrs(&ifa);

  for (ifaddrs *it = ifa; it != nullptr; it = it->ifa_next)
  {
    InterfaceConf interfaceConf;
    interfaceConf.ifa_name = std::string{it->ifa_name};
    auto family = it->ifa_addr->sa_family;

    interfaceConf.family =
        (family == AF_INET)
            ? "AF_INET"
            : (family == AF_INET6)
                  ? "AF_INET6"
                  : (family == AF_PACKET) ? "AF_PACKET" : "Unknown";

    if (family == AF_INET || family == AF_INET6)
    {
      int err = getnameinfo(it->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in)
                                                : sizeof(struct sockaddr_in6),
                            interfaceConf.host, NI_MAXHOST, nullptr, 0,
                            NI_NUMERICHOST);
      if (err != 0)
      {
        std::cerr << "getnameinfo() failed " << gai_strerror(err) << std::endl;
        continue;
      }
    }

    network.push_back(interfaceConf);
  }

  freeifaddrs(ifa);
}

void MachineManifest::loadHwConf()
{
  // load ram available
  struct sysinfo info;
  if (sysinfo(&info))
  {
    std::cout << "can't load ram info\n";
    return;
  }
  hwConf.ram = info.freeram;

  // load available cpu power
  size_t prevIdleTime = 0;
  size_t prevTotalTime = 0;

#if UNIT_TEST
  ifstream procStat("../test/cpu-mockfile");
#else
  ifstream procStat("/proc/stat");
#endif
  constexpr int procStatFileStartSkit{5};
  procStat.ignore(procStatFileStartSkit, ' ');  // skip cpu prefix
  std::vector<size_t> times;
  for (size_t time; procStat >> time; times.push_back(time))
    ;

  if (times.size() < 4)
  {
    std::cout << "Error obtaining cpu data\n";
  }

  constexpr int cpuPowerInputData{3};
  size_t idleTime = times[cpuPowerInputData];
  size_t totalTime = std::accumulate(times.begin(), times.end(), 0);

  float deltaIdle = idleTime - prevIdleTime;
  float deltaTotal = totalTime - prevTotalTime;
  float utilization = 100.0f * (1.0f - deltaIdle / deltaTotal);
  hwConf.cpu = 100 - static_cast<uint8_t>(utilization);
}


/// InterfaceConf serialization
void to_json(json& jsonObject, const InterfaceConf& interfaceConf)
{
    jsonObject = json{
            {"ifa_name", interfaceConf.ifa_name},
            {"family", interfaceConf.family},
            {"host", interfaceConf.host}
    };
}

/// InterfaceConf  deserialization
void from_json(const json& jsonObject, InterfaceConf& interfaceConf)
{
    jsonObject.at("ifa_name").get_to(interfaceConf.ifa_name);
    jsonObject.at("family").get_to(interfaceConf.family);
    jsonObject.at("host").get_to(interfaceConf.host);
}

/// HwConf serialization
void to_json(json& jsonObject, const HwConf& hwConf)
{
    jsonObject = json{
            {"ram", hwConf.ram},
            {"cpu", hwConf.cpu}
    };
}

/// HwConf  deserialization
void from_json(const json& jsonObject, HwConf& hwConf)
{
    jsonObject.at("ram").get_to(hwConf.ram);
    jsonObject.at("cpu").get_to(hwConf.cpu);
}

/// MachineState serialization
void to_json(json& jsonObject, const MachineManifest& machineManifest)
{
    jsonObject = json{
            {"network", machineManifest.network},
            {"hwConf", machineManifest.hwConf},
            {"states", machineManifest.states},
            {"adaptiveModules", machineManifest.adaptiveModules}
    };
}

/// MachineManifest deserialization
void from_json(const json& jsonObject, MachineManifest& machineManifest)
{
    jsonObject.at("network").get_to(machineManifest.network);
    jsonObject.at("hwConf").get_to(machineManifest.hwConf);
    jsonObject.at("states").get_to(machineManifest.states);
    jsonObject.at("adaptiveModules").get_to(machineManifest.adaptiveModules);
}

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

/// Process serialization
void to_json(json& jsonObject, const Process& process)
{
    jsonObject = json{
          {"Process_name", process.name},
          {"Mode_dependent_startup_configs", process.modeDependentStartupConf}
    };
}

/// Process deserialization
void from_json(const json& jsonObject, Process& process)
{
    jsonObject.at("Process_name").get_to(process.name);
    jsonObject.at("Mode_dependent_startup_configs")
      .get_to(process.modeDependentStartupConf);
}

void to_json(json& jsonObject, const Manifest& manifest)
{
  jsonObject = json{
    {"Application_manifest_id", manifest.manifestId},
    {"Process", manifest.processes}
  };
}

void from_json(const json& jsonObject, Manifest& manifest)
{
  jsonObject.at("Application_manifest_id").get_to(manifest.manifestId);
  jsonObject.at("Process").get_to(manifest.processes);
}


/// ApplicationManifest serialization
void to_json(json& jsonObject, const ApplicationManifest& applicationManifest)
{
    jsonObject = json{ "Application_manifest", applicationManifest.manifest };
}

/// ApplicationManifest deserialization
void from_json(const json& jsonObject, ApplicationManifest& applicationManifest)
{
  jsonObject.at("Application_manifest").get_to(applicationManifest.manifest);
}

} // namespace ExecutionManager
