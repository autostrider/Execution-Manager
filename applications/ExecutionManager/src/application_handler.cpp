#include "application_handler.hpp"

#include <csignal>
#include <exception>
#include <logger.hpp>
#include <fstream>
#include <thread>

namespace ExecutionManager
{

using std::runtime_error;

ApplicationHandler::ApplicationHandler(std::unique_ptr<IOsInterface> syscalls,
                                       std::string path)
        : corePath{std::move(path)},
          m_syscalls{std::move(syscalls)}
{ }

void ApplicationHandler::startProcess(const std::string &serviceName)
{
  execProcess(serviceName, SYSTEMCTL_START);
}

void ApplicationHandler::killProcess(const std::string &serviceName)
{
    execProcess(serviceName, SYSTEMCTL_STOP);
}

bool ApplicationHandler::isActiveProcess(const std::string &serviceName)
{
    const size_t maxPidLen = 128;
    const size_t size = 1;
    char pidline[maxPidLen];
    ::bzero(pidline, maxPidLen);
    int pidno = -1;

    auto pos = serviceName.find_first_of('_');
    std::string appName = serviceName.substr(pos+size);

    const std::string cmd = "pidof " + appName;

    FILE *fp = m_syscalls->popen(cmd.data(), "r");
    m_syscalls->fread(pidline, size, maxPidLen, fp);
    pidno = ::atoi(pidline);
    m_syscalls->pclose(fp);

    return pidno > 1;
}

void ApplicationHandler::execProcess(const std::string &processName,
                                const std::string& action) const
{
  pid_t process = m_syscalls->fork();

  if (-1 == process)
  {
    LOG << "Error forking the process" << strerror(errno);
  }
  else if (!process)
  {
    std::vector<std::string> arguments =
    {
      SYSTEMCTL,
      USER,
      action,
      processName + SERVICE_EXTENSION
    };
     auto applicationArgs = convertToNullTerminatingArgv(arguments);
     
     int res = m_syscalls->execvp(SYSTEMCTL.c_str(),
                      applicationArgs.data());

     if (res)
     {
       LOG << std::string{"Error occured creating process: "}
           << processName
           << " "
           << strerror(errno);
     }
  }
}

std::vector<char*>
ApplicationHandler::convertToNullTerminatingArgv(
  std::vector<std::string>& vectorToConvert) const
{
  std::vector<char*> outputVector;

  // include terminating sign, that not included in argv
  outputVector.reserve(vectorToConvert.size() + 1);

  for(auto& str: vectorToConvert)
  {
    outputVector.push_back(&str[0]);
  }

  // terminating sign
  outputVector.push_back(nullptr);

  return outputVector;
}

} // namespace ExecutionManager
