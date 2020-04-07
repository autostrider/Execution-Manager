#include "execution_manager_server.hpp"
#include "msm_handler.hpp"
#include <common.hpp>
#include <logger.hpp>
#include <server_socket.hpp>
#include <connection_factory.hpp>

#include <string>
#include <iostream>

using namespace constants;
using namespace common;

namespace ExecutionManagerServer
{

using std::string;

ExecutionManagerServer::ExecutionManagerServer
(ExecutionManager::ExecutionManager& application,
 ExecutionManager::MsmHandler msmHandler,
 std::unique_ptr<IServer> server)
  : m_em{application},
    m_msmHandler{msmHandler},
    m_server{std::move(server)}
{
    m_server->start();
    LOG << "Execution Manager server started...";

    isRunning = true;

    common::threadsMap[common::THREADS::EM_SERVER]->addMethod(
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

// void
// ExecutionManagerServer::reportApplicationState
// (ApplicationState context)
// {
//   ApplicationState state = context.getParams().getState();
//   std::string appName = context.getParams().getAppName().cStr();

//   m_em.reportApplicationState(appName,
//                               static_cast<ExecutionManager::AppState>(state));

// }

// ::kj::Promise<void>
// ExecutionManagerServer::register_(RegisterContext context)
// {
//   string newMachineClient = context.getParams().getAppName();
//   pid_t applicationPid = context.getParams().getPid();

//   if (m_msmHandler.registerMsm(applicationPid, newMachineClient))
//   {
//     context.getResults().setResult(StateError::kSuccess);
//   }
//   else
//   {
//     context.getResults().setResult(StateError::kInvalidRequest);
//   }

//   return kj::READY_NOW;
// }

// ::kj::Promise<void>
// ExecutionManagerServer::getMachineState(GetMachineStateContext context)
// {
//   context.getResults().setState(m_em.getMachineState());

//   context.getResults().setResult(StateError::);

//   return kj::READY_NOW;
// }

// ::kj::Promise<void>
// ExecutionManagerServer::setMachineState(SetMachineStateContext context)
// {
//   string state = context.getParams().getState().cStr();
//   pid_t applicationPid = context.getParams().getPid();

//   if (!m_msmHandler.checkMsm(applicationPid))
//   {
//     return kj::READY_NOW;
//   }

//   threadsMap.at(THREADS::EM_SERVER)->addMethod(
//               std::bind(&ExecutionManager::ExecutionManager::setMachineState,
//                         &m_em, state));

//   return kj::READY_NOW;
// }

} //namespace ExecutionManagerServer
