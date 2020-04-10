#include "i_application_state_client_wrapper.hpp"

namespace application_state
{

void ApplicationStateClientWrapper::setClient(std::unique_ptr<IClient> client)
{
  m_client.setClient(std::move(client));
}

void ApplicationStateClientWrapper::ReportApplicationState(ApplicationStateClient::ApplicationState state)
{
  m_client.ReportApplicationState(state);
}

}
