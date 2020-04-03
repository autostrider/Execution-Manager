#include "i_application_state_client_wrapper.hpp"

namespace application_state
{

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateClient::ApplicationState state)
{
  m_client.ReportApplicationState(state);
}

}
