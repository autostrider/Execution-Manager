#ifndef COMPONENT_CLIENT_H
#define COMPONENT_CLIENT_H

#include <component_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <functional>
#include <string>
#include <map>
#include <future>
#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>

namespace api {

using ComponentState = std::string;

using ComponentClientReturnType = StateManagement::ComponentClientReturnType;
using StateUpdateMode = StateManagement::StateUpdateMode;

enum class ComponentStates: uint8_t
{
    kOn = 0,
    kOff
};

static const ComponentState ComponentStateKOn = "kOn";
static const ComponentState ComponentStateKOff = "kOff";

class ComponentClient;

class ComponentServer
  : public StateManagement::StateManager::Server
{
public:
  using StateError = StateManagement::ComponentClientReturnType;
  ComponentServer(std::promise<ComponentState>&);
private:
 ::kj::Promise<void> setComponentState(SetComponentStateContext context);

private:
  std::promise<ComponentState>& m_eventPromise;
};

class ComponentClient
{
public:
  ComponentClient (const std::string &s, StateUpdateMode mode) noexcept;

  ComponentClientReturnType SetStateUpdateHandler (std::function<void(ComponentState const&)> f) noexcept;

  ComponentClientReturnType GetComponentState (ComponentState& state) noexcept;

  void ConfirmComponentState (ComponentState state, ComponentClientReturnType status) noexcept;

  void checkIfAnyEventsAvailable();

  ~ComponentClient();
private:  
  void startServer();
  bool eventReceived(std::future<ComponentState>& stateFuture);
 
private:
  capnp::EzRpcClient m_rpcClient;
  const std::string m_componentName;

  const StateUpdateMode m_updateMode;
  std::function<void(ComponentState const&)> m_stateUpdateHandler;

  StateManagement::Client m_stateManagementCap;

  kj::Own<kj::Thread> m_serverThread;
  kj::Own<kj::PromiseFulfiller<void>> m_listenFulfiller;
  kj::MutexGuarded<kj::Maybe<const kj::Executor&>> m_serverExecutor;
  std::promise<ComponentState> m_eventPromise;

  kj::Promise<void> m_serverStopPromise;

  kj::AsyncIoProvider::PipeThread m_event;
};

} // namespace api

#endif // COMPONENT_CLIENT_H
