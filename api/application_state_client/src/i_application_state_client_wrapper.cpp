#include <i_application_state_client_wrapper.h>

namespace api {

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateManagement::ApplicationState state)
{
    m_client.ReportApplicationState(state);
}

}
