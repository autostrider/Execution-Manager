#ifndef APPLICATION_STATE_CLIENT_WRAPPER_H
#define APPLICATION_STATE_CLIENT_WRAPPER_H

#include <asc_for_wrapper.hpp>

namespace application_state
{

class IApplicationStateClientWrapper
{
public:
    virtual void ReportApplicationState(ApplicationStateClient::ApplicationState state) = 0;
    virtual ~IApplicationStateClientWrapper() noexcept(false) {}
};

class ApplicationStateClientWrapper : public IApplicationStateClientWrapper
{
public:
    void setClient(std::unique_ptr<IClient> client);
    void ReportApplicationState(ApplicationStateClient::ApplicationState state) override;
private:
    AscForWrapper m_client;
};

}


#endif
