#include "common.hpp"

std::string
getServiceName(const std::string& appName, const std::string& procName)
{
  return appName + "_" + procName;
}