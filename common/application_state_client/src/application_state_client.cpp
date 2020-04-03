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
  : m_client((IPC_PROTOCOL + EM_SOCKET_NAME),
             std::make_unique<socket_handler::ClientSocket>()),
    m_pid(getpid())
{}

void ApplicationStateClient::ReportApplicationState(ApplicationState state)
{
  ApplicationStateManagement::registerComponent context;
  auto fullPath = getAppBinaryPath(m_pid);

  context.set_state(state);
  context.set_appname(parseServiceName(fullPath));

  google::protobuf::Any sendData;

  sendData.PackFrom(context);
}

} // namespace application_state
