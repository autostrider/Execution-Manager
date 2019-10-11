#ifndef COMPONENT_CLIENT_H
#define COMPONENT_CLIENT_H

#include <component_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <functional>
#include <string>
#include <map>

namespace api {

using ComponentState = std::string;

using ComponentClientReturnType = StateManagement::ComponentClientReturnType;

enum class StateUpdateMode : uint8_t
{
  kPoll = 0,
  kEvent
};

enum class ComponentStates: uint8_t
{
    kOn = 0,
    kOff
};

const std::map<ComponentStates, ComponentState> ComponentStatesToString =
{
    {ComponentStates::kOn, "kOn"},
    {ComponentStates::kOff, "kOff"}
};

class ComponentClient
{
public:
  ComponentClient
  (const std::string &s, StateUpdateMode mode) noexcept;

  ComponentClientReturnType SetStateUpdateHandler
  (std::function<void(ComponentState const&)> f) noexcept;

  ComponentClientReturnType GetComponentState
  (ComponentState& state) noexcept;

  void ConfirmComponentState
  (ComponentState state, ComponentClientReturnType status) noexcept;
private:
   capnp::EzRpcClient m_client;

   const std::string componentName;

   const StateUpdateMode updateMode;
   std::function<void(ComponentState const&)> updateHandler;
};

} // namespace api

#endif // COMPONENT_CLIENT_H
