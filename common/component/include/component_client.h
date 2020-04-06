#ifndef COMPONENT_CLIENT_H
#define COMPONENT_CLIENT_H

#include <component_state_management.pb.h>
#include <functional>
#include <string>
#include <map>
#include <future>

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
  ComponentClient (const std::string &s, StateUpdateMode mode) noexcept;

  ComponentClientReturnType GetComponentState (ComponentState& state) noexcept;
  void ConfirmComponentState (ComponentState state, ComponentClientReturnType status) noexcept;

  ~ComponentClient();
 
private:
  capnp::EzRpcClient m_rpcClient;
  const std::string m_componentName;

  const StateUpdateMode m_updateMode;
  std::function<void(ComponentState const&)> m_stateUpdateHandler;

  kj::Own<kj::Thread> m_serverThread;
  kj::Own<kj::PromiseFulfiller<void>> m_listenFulfiller;
  kj::MutexGuarded<kj::Maybe<const kj::Executor&>> m_serverExecutor;
  std::promise<ComponentState> m_eventPromise;

  kj::Promise<void> m_serverStopPromise;

  StateManagement::Client m_stateManagementCap;

  kj::AsyncIoProvider::PipeThread m_event;
};

} // namespace component_client

#endif // COMPONENT_CLIENT_H
