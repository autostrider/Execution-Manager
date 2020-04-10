#include "application_state_client.h"
#include <constants.hpp>
#include <common.hpp>
#include <client_socket.hpp>

#include <application_state_management.pb.h>

using namespace constants;
using namespace common;

namespace application_state
{

ApplicationStateClient::ApplicationStateClient()
  : m_pid(getpid())
{}

void ApplicationStateClient::setClient(std::unique_ptr<IClient> client)
{
  m_client = std::move(client);
  m_client->connect();
}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  ApplicationStateManagement::ReportApplicationState context;
  auto fullPath = getAppBinaryPath(m_pid);

  context.set_state(state);
  context.set_appname(parseServiceName(fullPath));

  m_client->sendMessage(context);
}

} // namespace application_state
