#include "application_state_client.h"
#include <constants.hpp>

namespace api 
{
ApplicationStateClient::ApplicationStateClient()
  : m_client(SOCKET_NAME.c_str()),
    m_pid(getpid())
{}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  auto& waitScope = m_client.getWaitScope();

  auto cap =
    m_client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);

  request.setPid(m_pid);

  auto promise = request.send();
  promise.wait(waitScope);
}

} // namespace api
