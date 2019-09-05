#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H
#include <sys/types.h>
#include <unistd.h>
#include <application_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <json.hpp>

namespace api {

class ApplicationStateClient
{
public:
  ApplicationStateClient();

  using ApplicationState = ::ApplicationStateManagement::ApplicationState;

  void ReportApplicationState(ApplicationState state);
private:
  capnp::EzRpcClient m_client;

  pid_t m_pid;
};

} // namespace api
#endif // APPLICATION_STATE_CLIENT_H
