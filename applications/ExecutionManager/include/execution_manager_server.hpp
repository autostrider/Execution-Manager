#ifndef EXECUTION_MANAGER_SERVER_HPP
#define EXECUTION_MANAGER_SERVER_HPP

#include "execution_manager.hpp"
#include "msm_handler.hpp"
#include <server.hpp>

#include <thread>
#include <atomic>

namespace ExecutionManagerServer
{


class ExecutionManagerServer : public base_server::Server
{
public:
  explicit
  ExecutionManagerServer
    (ExecutionManager::ExecutionManager& application,
     ExecutionManager::MsmHandler msmHandler);
  ~ExecutionManagerServer();
private:
  void reportApplicationState(ApplicationState context);

  // ::kj::Promise<void>
  // register_(RegisterContext context) override;

  // ::kj::Promise<void>
  // getMachineState(GetMachineStateContext context) override;

  // ::kj::Promise<void>
  // setMachineState(SetMachineStateContext context) override;

  // ::kj::Promise<void>
  // registerComponent(RegisterComponentContext context) override;

  // ::kj::Promise<void>
  // getComponentState(GetComponentStateContext context) override;

  // ::kj::Promise<void>
  // confirmComponentState(ConfirmComponentStateContext context) override;

private:
  ExecutionManager::ExecutionManager& m_em;
  ExecutionManager::MsmHandler m_msmHandler;
  std::thread m_setMachineStateThread;
  std::atomic<bool> isRunning{false};
};

} // namespace ExecutionManagerServer

#endif // EXECUTION_MANAGER_SERVER_HPP
