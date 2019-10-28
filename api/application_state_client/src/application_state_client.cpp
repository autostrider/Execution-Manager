#include "application_state_client.h"
#include <constants.hpp>
#include <fstream>
#include <logger.hpp>
#include <exception>

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
  request.setAppName(getAppName());
  request.setPid(m_pid);
  auto promise = request.send();
  try
  {
    promise.wait(waitScope);
  }
  catch (std::exception& e)
  {
    LOG << e.what();
  }
}

std::string ApplicationStateClient::getAppName()
{
  static constexpr auto procPath = "/proc/";
  static constexpr auto cmdName = "/cmdline";

  std::ifstream data {procPath + std::to_string(m_pid) + cmdName};
  std::string fullCmd;
  data >> fullCmd;

  const auto lastSlash = fullCmd.find_last_of("/");

  return fullCmd.substr(lastSlash + 1);
}

} // namespace api
