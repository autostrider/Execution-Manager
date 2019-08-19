#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H
#include <application_state_management.capnp.h>
#include <capnp/ez-rpc.h>

namespace api {

class ApplicationStateClient
{
public:
  ApplicationStateClient();
  ~ApplicationStateClient() = default;

  using ApplicationState = ::ApplicationStateManagement::ApplicationState;

  void reportApplicationState(ApplicationState state);
private:
  capnp::EzRpcClient client;
};

} // namespace api
#endif // APPLICATION_STATE_CLIENT_H