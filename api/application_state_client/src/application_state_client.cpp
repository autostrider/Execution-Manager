#include "application_state_client.h"

namespace api {
namespace {
  const char* executionManagerAddr = "unix:/tmp/execution_management";
}

ApplicationStateClient::ApplicationStateClient()
  : m_client(executionManagerAddr),
    m_pid(getpid())
{}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  auto cap =
    m_client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);
  request.setPid(m_pid);

  request.send().ignoreResult().wait(m_client.getWaitScope());
}

} // namespace api
