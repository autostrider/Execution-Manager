#include "application_state_client.h"
#include <constants.hpp>
#include <common.hpp>

using namespace constants;
using namespace common;

namespace application_state
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

  auto fullPath = getAppBinaryPath(m_pid);
  request.setAppName(parseServiceName(fullPath));
  auto promise = request.send();
  promise.wait(waitScope);
}

} // namespace application_state
