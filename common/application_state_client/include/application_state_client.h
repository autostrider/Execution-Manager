#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H

#include <client.hpp>
#include <enums.pb.h>

#include <unistd.h>

using namespace base_client;

namespace application_state
{

class ApplicationStateClient
{
public:
  ApplicationStateClient();

  using ApplicationState = enums::ApplicationState;

  void ReportApplicationState(ApplicationState state);

private:
  Client m_client;
  pid_t m_pid;
};

} // namespace application_state
#endif // APPLICATION_STATE_CLIENT_H
