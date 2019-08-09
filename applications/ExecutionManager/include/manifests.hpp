#ifndef MANIFESTS_HPP
#define MANIFESTS_HPP

#include "application_state_client.h"

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

namespace ExecutionManager
{

using std::ifstream;
using std::string;
using std::vector;

/**
 * @brief Enum with available machine states.
 */
enum class MachineStates 
{ 
    kInit, 
    kRunning, 
    kShutdown, 
    kRestart = -1 
};

/**
 * @brief Struct, that holds network interface configuration.
 * 
 * @field ifa_name: name of interface
 * @field family: family of interface
 */
struct InterfaceConf 
{
  string ifa_name;
  string family;
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
  vector<InterfaceConf> network;
  HwConf hwConf;
  vector<MachineStates> states;
  vector<string> adaptiveModules;

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



/**
 * @brief The Process struct
 */
struct Process
{
  /// Name of executable.
  string name;

  /// Available machine states to run the executable.
  vector<MachineStates> startMachineStates;

};

/**
 * @brief Application manifest structure.
 */
struct ApplicationManifest 
{
  /// Name of Adaptive Application.
  string name;

  /// Vector of executables of application
  vector<Process> processes;
};

} // namespace ExecutionManager

#include "json_converters.hpp"

void ExecutionManager::MachineManifest::init()
{
  loadHwConf();
  loadNetworkConf();
  states = {MachineStates::kRunning};
  adaptiveModules = {};
}

void ExecutionManager::MachineManifest::loadNetworkConf()
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

void ExecutionManager::MachineManifest::loadHwConf()
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

#endif // MANIFESTS_HPP
