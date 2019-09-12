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
  auto& waitScope = m_client.getWaitScope();

  auto cap =
    m_client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);

  request.setPid(m_pid);

  auto promise = request.send();
  promise.wait(waitScope);
}

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateManagement::ApplicationState state)
{
      m_client.ReportApplicationState(state);
}
} // namespace api
