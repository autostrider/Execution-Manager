#include "component_client.h"
#include <constants.hpp>
#include <client.hpp>
#include <client_socket.hpp>
#include <logger.hpp>

#include <any.pb.h>
#include <execution_management.pb.h>

#include <memory>
#include <thread>

using namespace constants;

namespace component_client
{

ComponentClient::ComponentClient
(const std::string& componentName, 
 StateUpdateMode mode) noexcept
: m_componentName(componentName),
  m_updateMode(mode),
  m_client((IPC_PROTOCOL + EM_SOCKET_NAME), std::make_unique<socket_handler::ClientSocket>())
{
  StateManagement::RegisterComponent context;
  context.set_component(m_componentName);
  context.set_mode(m_updateMode);

  m_client.sendMessage(context);
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  // auto request = m_stateManagementCap.getComponentStateRequest();
  // request.setComponent(m_componentName);

  // auto result = request.send().wait(m_rpcClient.getWaitScope());
  // state = result.getState();

  // return result.getResult();
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  StateManagement::ConfirmComponentState context;
  context.set_component(m_componentName);
  context.set_state(state);
  context.set_status(status);

  m_client.sendMessage(context);
}

ComponentState ComponentClient::setStateUpdateHandler() noexcept
{
  google::protobuf::Any any;
  std::string data;

  do
  {    
    if (m_client.receive(data) > 0)
    {
      any.ParseFromString(data);

      if(any.Is<ExecutionManagement::setComponentState>())
      {
        ExecutionManagement::setComponentState comp;
        any.UnpackTo(&comp);
            
        return comp.state();
      }

      any = {};
      data = {};
    }
  } while (true);
}

} // namespace component_client
