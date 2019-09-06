#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H
#include <application_state_management.capnp.h>
#include <capnp/ez-rpc.h>

namespace api {

class ApplicationStateClient
{
public:
  ApplicationStateClient();

  using ApplicationState = ::ApplicationStateManagement::ApplicationState;

  void ReportApplicationState(ApplicationState state);
private:
  capnp::EzRpcClient client;
};

} // namespace api
#endif // APPLICATION_STATE_CLIENT_H
