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

namespace manifests 
{

using std::ifstream;
using std::string;
using std::vector;

enum class MachineStates 
{ 
    kInit, 
    kRunning, 
    kShutdown, 
    kRestart = -1 
};

struct InterfaceConf 
{
  string ifa_name;
  string family;
  char host[NI_MAXHOST];
};

struct HwConf 
{
  uint64_t ram;
  uint8_t cpu;
};

struct MachineManifest 
{
 public:
  vector<InterfaceConf> network;
  HwConf hwConf;
  vector<MachineStates> states;
  vector<std::string> adaptiveModules;

 public:
  void init();

 private:
  void loadNetworkConf();
  void loadHwConf();
};

enum class AppStates 
{
    kInitializing, 
    kRunning, 
    kShuttingdown
};

struct StartupDeps 
{
  std::string name;
  AppStates reqState;
};

struct ApplicationManifest 
{
  std::string name;
  std::string version;
  std::vector<MachineStates> machineStates;
  std::vector<StartupDeps> deps;
};

} // namespace manifests

#include "jsonConerters.hpp"

void manifests::MachineManifest::init()
{
  loadHwConf();
  loadNetworkConf();
  states = {MachineStates::kRunning};
  adaptiveModules = {};
}

void manifests::MachineManifest::loadNetworkConf()
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

void manifests::MachineManifest::loadHwConf()
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

  ifstream procStat("/proc/stat");  // skip cpu prefix
  procStat.ignore(5, ' ');
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