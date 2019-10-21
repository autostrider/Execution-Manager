#include "execution_manager_server.hpp"
#include "msm_handler.hpp"
#include <kj/async-io.h>
#include <string>
#include <iostream>
#include <logger.hpp>

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
  std::string appName = context.getParams().getAppName().cStr();
  m_em.reportApplicationState(applicationPid, appName,
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
    context.getResults().setResult(StateError::K_SUCCESS);
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::getMachineState(GetMachineStateContext context)
{
  // pid_t applicationPid = context.getParams().getPid();

  context.getResults().setState(m_em.getMachineState());

  context.getResults().setResult(StateError::K_SUCCESS);

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

} //namespace ExecutionManagerServer
