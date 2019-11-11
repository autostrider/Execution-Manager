#include "application_state_client.h"
#include <constants.hpp>
#include <common.hpp>
#include <fstream>
#include <logger.hpp>
#include <exception>
#include <algorithm>
#include <sstream>

namespace 
{

std::string getAppName(pid_t appPid)
{
  static constexpr auto procPath = "/proc/";
  static constexpr auto cmdName = "/cmdline";
  static constexpr auto delimiter = '/';

  std::ifstream data {procPath + std::to_string(appPid) + cmdName};
  std::string fullCmd;
  data >> fullCmd;

  std::reverse(fullCmd.begin(), fullCmd.end());
  std::stringstream info{fullCmd};
  std::string processName;
  std::getline(info, processName, delimiter);
  std::reverse(processName.begin(), processName.end());
  std::string appName;
  // According to our spec, we have /AppName/process/procName
  // so we skip process word
  std::getline(info, appName, delimiter);
  // actual app name
  std::getline(info, appName, delimiter);
  std::reverse(appName.begin(), appName.end());

  return getServiceName(appName, processName);
}

}
namespace api
{
ApplicationStateClient::ApplicationStateClient()
  : m_client(IPC_PROTOCOL + EM_SOCKET_NAME),
    m_pid(getpid())
{}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  auto& waitScope = m_client.getWaitScope();
  auto cap =
    m_client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);
  request.setAppName(getAppName(m_pid));
  auto promise = request.send();
  promise.wait(waitScope);
}

} // namespace api
