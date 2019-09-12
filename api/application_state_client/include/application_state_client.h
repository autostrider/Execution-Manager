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
  capnp::EzRpcClient m_client;

  pid_t m_pid;
};

class IApplicationStateClientWrapper
{
public:
    virtual void ReportApplicationState(ApplicationStateManagement::ApplicationState state) = 0;
};

class ApplicationStateClientWrapper : public IApplicationStateClientWrapper
{
public:
    void ReportApplicationState(ApplicationStateManagement::ApplicationState state) override;
private:
    ApplicationStateClient m_client;
};

} // namespace api
#endif // APPLICATION_STATE_CLIENT_H
