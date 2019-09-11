#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H
#include <application_state_management.capnp.h>
#include <capnp/ez-rpc.h>

namespace api {

class StateClient
{
public:
    StateClient();
    virtual ~StateClient() = default;
    using ApplicationState = ::ApplicationStateManagement::ApplicationState;
    virtual void ReportApplicationState(ApplicationState state){}

protected:
    capnp::EzRpcClient client;
};

class ApplicationStateClient : public StateClient
{
public:
  void ReportApplicationState(ApplicationState state) override;

};

} // namespace api
#endif // APPLICATION_STATE_CLIENT_H
