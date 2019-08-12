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
  states = {MachineStates::kRunning};
  adaptiveModules = {};
}

void MachineManifest::loadNetworkConf()
{
  ifaddrs *ifa;
  getifaddrs(&ifa);

  for (ifaddrs *it = ifa; it != nullptr; it = it->ifa_next)
  {
    InterfaceConf interfaceConf;
    interfaceConf.ifa_name = string{it->ifa_name};
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
  procStat.ignore(5, ' ');  // skip cpu prefix
  vector<size_t> times;
  for (size_t time; procStat >> time; times.push_back(time))
    ;

  if (times.size() < 4)
  {
    std::cout << "Error obtaining cpu data\n";
  }

  size_t idleTime = times[3];
  size_t totalTime = std::accumulate(times.begin(), times.end(), 0);

  float deltaIdle = idleTime - prevIdleTime;
  float deltaTotal = totalTime - prevTotalTime;
  float utilization = 100.0 * (1.0 - deltaIdle / deltaTotal);
  hwConf.cpu = 100 - utilization;
}


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
