#include "application_state_client.h"

namespace api {
namespace {
  const char* executionManagerAddr = "unix:/tmp/execution_management";
}

ApplicationStateClient::ApplicationStateClient()
  : client(executionManagerAddr)
{}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  auto& waitScope = client.getWaitScope();

  auto cap =
    client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);

  auto promise = request.send();
  promise.wait(waitScope);
}

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateManagement::ApplicationState state)
{
    m_client.ReportApplicationState(state);
}

} // namespace api
