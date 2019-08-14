#include "application_state_client.h"

namespace api {

void ApplicationStateClient::reportApplicationState(ApplicationState state)
{
  capnp::EzRpcClient client("unix:/tmp/execution_management", 5923);
  auto& waitScope = client.getWaitScope();

  ApplicationStateManagement::Client cap = 
    client.getMain<ApplicationStateManagement>();

  auto request = cap.reportApplicationStateRequest();
  request.setState(state);

  auto promise = request.send();

  promise.wait(waitScope);
}

} // namespace api