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

#include <application_state_client.h>
#include <machine_state_client.h>

namespace ExecutionManager
{

using std::ifstream;
using std::string;
using std::vector;

using api::MachineStates;
using api::ApplicationState;

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

#endif // MANIFESTS_HPP

#include "json_converters.hpp"
