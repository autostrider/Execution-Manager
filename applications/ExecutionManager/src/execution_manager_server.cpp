#include "execution_manager_server.hpp"

#include <iostream>

namespace ExecutionManagerServer
{

ExecutionManagerServer::ExecutionManagerServer
(ExecutionManager::ExecutionManager& application)
  : m_em(application)
{
  std::cout << "Execution Manager server started..." << std::endl;
}

::kj::Promise<void>
ExecutionManagerServer::reportApplicationState(ReportApplicationStateContext context)
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
  std::string newMachineClient = context.getParams().getAppName();
  pid_t applicationPid = context.getParams().getPid();

  if (m_em.registerMachineStateClient(applicationPid, newMachineClient))
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
  pid_t applicationPid = context.getParams().getPid();

  context.getResults().setState(m_em.getMachineState(applicationPid));

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::setMachineState(SetMachineStateContext context)
{
  std::string state = context.getParams().getState().cStr();
  pid_t applicationPid = context.getParams().getPid();

  if (m_em.setMachineState(applicationPid, state))
  {
    context.getResults().setResult(StateError::K_SUCCESS);
  }

  context.getResults().setResult(StateError::K_INVALID_STATE);

  return kj::READY_NOW;
}

} //namespace ExecutionManagerServer