#include "i_application_state_client_wrapper.hpp"

namespace api
{

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateManagement::ApplicationState state)
{
  m_client.ReportApplicationState(state);
}

}
