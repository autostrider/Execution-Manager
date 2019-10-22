#ifndef MACHINE_STATE_CLIENT_H
#define MACHINE_STATE_CLIENT_H

#include <machine_state_management.capnp.h>
#include <capnp/ez-rpc.h>
#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>
#include <future>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace api {

class MachineStateClient;

class MachineStateServer
  : public MachineStateManagement::MachineStateManager::Server
{
public:
  using StateError = MachineStateManagement::StateError;
  MachineStateServer(std::promise<StateError>& promise);
private:
  ::kj::Promise<void>
  confirmStateTransition(ConfirmStateTransitionContext context);
private:
  std::promise<StateError>& m_promise;
};

class MachineStateClient
{
public:
  MachineStateClient(std::string path);
  ~MachineStateClient();

  // K_SUCCESS
  // K_INVALID_STATE
  // K_INVALID_REQUEST
  // K_TIMEOUT
  using StateError = MachineStateServer::StateError;

  StateError Register(std::string appName, std::uint32_t timeout);
  StateError GetMachineState(std::uint32_t timeout, std::string& state);
  StateError SetMachineState(std::string state, std::uint32_t timeout);
  StateError waitForConfirm(std::uint32_t timeout);
private:
  void startServer();

private:
  capnp::EzRpcClient m_client;
  MachineStateManagement::Client m_clientApplication;
  kj::Timer& m_timer;

  kj::AsyncIoProvider::PipeThread m_serverThread;
  kj::Own<kj::PromiseFulfiller<void>> m_listenFulfiller;
  kj::MutexGuarded<kj::Maybe<const kj::Executor&>> m_serverExecutor;

  pid_t m_pid;
  std::promise<StateError> m_promise;
};

}
#endif // MACHINE_STATE_CLIENT_H
