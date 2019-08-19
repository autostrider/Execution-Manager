#include "application_state_client.h"

namespace api {

ApplicationStateClient::ApplicationStateClient()
  : client("unix:/tmp/execution_management", 5923)
{}

void ApplicationStateClient::reportApplicationState(ApplicationState state)
{
  auto& waitScope = client.getWaitScope();

  ApplicationStateManagement::Client cap =
    client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);

  auto promise = request.send();
  promise.wait(waitScope);
}

} // namespace api