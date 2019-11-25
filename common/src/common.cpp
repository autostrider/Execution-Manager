#include "common.hpp"

#include <fstream>

std::string
getServiceName(const std::string& appName, const std::string& procName)
{
  return appName + "_" + procName;
}

std::string getAppBinaryPath(pid_t appPid)
{
  static constexpr auto procPath = "/proc/";
  static constexpr auto cmdName = "/cmdline";

  std::ifstream data {procPath + std::to_string(appPid) + cmdName};
  std::string fullCmd;
  data >> fullCmd;

  return fullCmd;
}

std::string parseServiceName(const std::string& path)
{
  static constexpr auto delimiter = '/';

  static const auto processesSize = std::string("/processes/").size();
  auto currPos = path.find_last_of(delimiter);
  std::string processName = path.substr(currPos + 1);
  
  auto prevPos = currPos - processesSize;
  currPos = path.find_last_of(delimiter, prevPos);
  std::string appName = path.substr(currPos + 1, prevPos - currPos);

  return getServiceName(appName, processName);
}
  