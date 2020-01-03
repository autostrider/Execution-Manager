#include "execution_manager_server.hpp"
#include "msm_handler.hpp"
#include <common.hpp>
#include <logger.hpp>

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
  LOG << "Execution Manager server started...";

    isRunning = true;

    threadsMap[THREADS::EM_SERVER]->addMethod(
                std::bind(&ExecutionManager::ExecutionManager::setMachineState,
                          &m_em, MACHINE_STATE_STARTUP));

    m_setMachineStateThread = std::thread([&]()
    {
        while (isRunning)
        {
            threadsMap.at(THREADS::EM_SERVER)->getExecutor();
        }
    });
}

ExecutionManagerServer::~ExecutionManagerServer()
{
    isRunning = false;
    if (m_setMachineStateThread.joinable())
    {
        m_setMachineStateThread.join();
    }
}

::kj::Promise<void>
ExecutionManagerServer::reportApplicationState
(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();
  std::string appName = context.getParams().getAppName().cStr();

  m_em.reportApplicationState(appName,
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

  threadsMap.at(THREADS::EM_SERVER)->addMethod(
              std::bind(&ExecutionManager::ExecutionManager::setMachineState,
                        &m_em, state));

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManagerServer::registerComponent(RegisterComponentContext context)
{
  m_em.registerComponent(context.getParams().getComponent().cStr(), 
                         context.getParams().getMode());

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
