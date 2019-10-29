#ifndef COMPONENT_CLIENT_H
#define COMPONENT_CLIENT_H

#include <functional>
#include <string>
#include <map>

namespace api {

using ComponentState = std::string;

enum class ComponentClientReturnType : uint8_t
{
  kSuccess = 0,
  kGeneralError,
  kPending,
  kInvalid,
  kUnchanged = 6
};

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

static const ComponentState ComponentStateKOn = "kOn";
static const ComponentState ComponentStateKOff = "kOff";

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
};

} // namespace api

#endif // COMPONENT_CLIENT_H
