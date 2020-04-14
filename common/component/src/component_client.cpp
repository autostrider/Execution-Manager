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
  m_updateMode(mode)
{}

void ComponentClient::setClient(std::unique_ptr<IClient> client)
{
  m_client = std::move(client);
  m_client->connect();
}

ComponentClientReturnType
ComponentClient::GetComponentState
(ComponentState& state) noexcept
{
  StateManagement::GetComponentState context;
  context.set_component(m_componentName);

  m_client->sendMessage(context);

  google::protobuf::Any any;
  std::string data;
  int recvData = -1;

  do
  {
    recvData = m_client->receive(data);

    if (recvData > 0)
    {
      any.ParseFromString(data);

      if(any.Is<ExecutionManagement::resultGetComponentState>())
      {
        ExecutionManagement::resultGetComponentState comp;
        any.UnpackTo(&comp);

        state = comp.state();
            
        return comp.result();
      }

      any = {};
      data = {};
    }
  } while (recvData <= 0);
}

void
ComponentClient::ConfirmComponentState
(ComponentState state, ComponentClientReturnType status) noexcept
{
  StateManagement::ConfirmComponentState context;
  context.set_component(m_componentName);
  context.set_state(state);
  context.set_status(status);

  m_client->sendMessage(context);
}

ComponentClientReturnType ComponentClient::setStateUpdateHandler(std::function<void(ComponentState const&)> f) noexcept
{
  m_stateUpdateHandler = f;

  return registerComponent();
}

ComponentClientReturnType ComponentClient::registerComponent()
{
  StateManagement::RegisterComponent context;
  context.set_component(m_componentName);
  context.set_mode(m_updateMode);

  m_client->sendMessage(context);

  google::protobuf::Any any;
  std::string data;
  int recvData = -1;

  do
  {
    recvData = m_client->receive(data);
    
    if (recvData > 0)
    {
      any.ParseFromString(data);

      ExecutionManagement::resultRegisterComponent comp;
      any.UnpackTo(&comp);

      return comp.result();

      any = {};
      data = {};
    }
  } while (recvData <= 0);
}

void ComponentClient::checkIfAnyEventsAvailable()
{
  google::protobuf::Any any;
  std::string data;

  if (m_client->receive(data) > 0)
  {
    any.ParseFromString(data);

    if(any.Is<ExecutionManagement::setComponentState>())
    {
      ExecutionManagement::setComponentState comp;
      any.UnpackTo(&comp);
            
      m_stateUpdateHandler(comp.state());
    }

    any = {};
    data = {};
  }  
}

} // namespace component_client
