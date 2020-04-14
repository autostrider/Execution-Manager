#ifndef COMPONENT_CLIENT_H
#define COMPONENT_CLIENT_H

#include <client.hpp>
#include <component_state_management.pb.h>

#include <functional>
#include <string>

using namespace base_client;

namespace component_client
{

using ComponentState = std::string;

using ComponentClientReturnType = enums::ComponentClientReturnType;
using StateUpdateMode = enums::StateUpdateMode;

enum class ComponentStates: uint8_t
{
    kOn = 0,
    kOff
};

static const ComponentState ComponentStateKOn = "kOn";
static const ComponentState ComponentStateKOff = "kOff";

class ComponentClient;

class ComponentClient
{
public:
  ComponentClient (const std::string &componentName, StateUpdateMode mode) noexcept;

  ComponentClientReturnType GetComponentState (ComponentState& state) noexcept;
  void ConfirmComponentState (ComponentState state, ComponentClientReturnType status) noexcept;
  ComponentClientReturnType setStateUpdateHandler(std::function<void(ComponentState const&)> f) noexcept;
  void checkIfAnyEventsAvailable();

  ~ComponentClient() = default;

protected:
  void setClient(std::unique_ptr<IClient> client);

private:
  ComponentClientReturnType registerComponent();
 
private:
  const std::string m_componentName;
  const StateUpdateMode m_updateMode;
  std::unique_ptr<IClient> m_client;
  std::function<void(ComponentState const&)> m_stateUpdateHandler;
};

} // namespace component_client

#endif // COMPONENT_CLIENT_H
