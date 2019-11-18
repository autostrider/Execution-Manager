#include "execution_manager_server.hpp"
#include "msm_handler.hpp"
#include <kj/async-io.h>
#include <string>
#include <iostream>

namespace ExecutionManagerServer
{
using ApplicationState = ::ApplicationStateManagement::ApplicationState;
using StateError = ::MachineStateManagement::StateError;
using std::string;

ExecutionManagerServer::ExecutionManagerServer
(ExecutionManager::ExecutionManager& application,
 ExecutionManager::MsmHandler msmHandler)
  : m_em{application},
    m_msmHandler{msmHandler}
{
  std::cout << "Execution Manager server started..." << std::endl;

  m_em.start();
}

::kj::Promise<void>
ExecutionManagerServer::reportApplicationState
  (ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();
  pid_t applicationPid = context.getParams().getPid();

  m_em.reportApplicationState(applicationPid,
    static_cast<ExecutionManager::AppState>(state));

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::register_(RegisterContext context)
{
  string newMachineClient = context.getParams().getAppName();
  pid_t applicationPid = context.getParams().getPid();

  if (m_msmHandler.registerMsm(applicationPid, newMachineClient))
  {
    context.getResults().setResult(StateError::kSuccess);
  }
  else
  {
    context.getResults().setResult(StateError::kInvalidRequest);
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::getMachineState(GetMachineStateContext context)
{
  // pid_t applicationPid = context.getParams().getPid();

  context.getResults().setState(m_em.getMachineState());

  context.getResults().setResult(StateError::kSuccess);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::setMachineState(SetMachineStateContext context)
{
  string state = context.getParams().getState().cStr();
  pid_t applicationPid = context.getParams().getPid();

  if (!m_msmHandler.checkMsm(applicationPid))
  {
    return kj::READY_NOW;
  }

  m_em.setMachineState(state);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::registerComponent(RegisterComponentContext context)
{
  m_em.registerComponent(context.getParams().getComponent().cStr());

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::getComponentState(GetComponentStateContext context)
{
  std::string component = context.getParams().getComponent().cStr();

  ComponentState state;
  auto result = m_em.getComponentState(component, state);

  context.getResults().setState(state);
  context.getResults().setResult(result);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::confirmComponentState
(ConfirmComponentStateContext context)
{
  std::string component = context.getParams().getComponent().cStr();
  ComponentState state = context.getParams().getState().cStr();
  ComponentClientReturnType status = context.getParams().getStatus();

  m_em.confirmComponentState(component, state, status);

  return kj::READY_NOW;
}

} //namespace ExecutionManagerServer