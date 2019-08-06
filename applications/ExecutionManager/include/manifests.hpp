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

#endif // MANIFESTS_HPP