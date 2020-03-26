#ifndef APPLICATION_STATE_CLIENT_WRAPPER_H
#define APPLICATION_STATE_CLIENT_WRAPPER_H

#include <application_state_client.h>
#include <application_state_management.capnp.h>

namespace application_state
{

class IApplicationStateClientWrapper
{
public:
    virtual void ReportApplicationState(ApplicationStateManagement::ApplicationState state) = 0;
    virtual ~IApplicationStateClientWrapper() noexcept(false) {}
};

class ApplicationStateClientWrapper : public IApplicationStateClientWrapper
{
public:
    void ReportApplicationState(ApplicationStateManagement::ApplicationState state) override;
private:
    ApplicationStateClient m_client;
};

}


#endif
