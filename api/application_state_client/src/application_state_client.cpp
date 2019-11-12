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
  static constexpr int processesSize = 11;

  std::ifstream data {procPath + std::to_string(appPid) + cmdName};
  std::string fullCmd;
  data >> fullCmd;

  int prevPos = fullCmd.length();
  int currPos = fullCmd.find_last_of(delimiter, currPos);
  std::string processName = fullCmd.substr(currPos + 1);
  
  prevPos = currPos - processesSize;
  currPos = fullCmd.find_last_of(delimiter, prevPos);
  std::string appName = fullCmd.substr(currPos + 1, prevPos - currPos);

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
