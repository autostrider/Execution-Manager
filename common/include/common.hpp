#ifndef COMMON
#define COMMON

#include <string>

std::string 
getServiceName(const std::string& appName, const std::string& procName);

std::string getAppBinaryPath(pid_t pid);

std::string parseServiceName(const std::string& binaryPath);

#endif // COMMON