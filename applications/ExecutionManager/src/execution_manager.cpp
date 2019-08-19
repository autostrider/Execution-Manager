#include "execution_manager.hpp"

namespace ExecutionManager {

using std::cout;
using std::endl;
using std::string;

::kj::Promise<void>
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();

  cout << "State #" << static_cast<uint16_t>(state)
            << " received"
            << endl;

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::register_(RegisterContext context)
{
  string newMachineClient = context.getParams().getAppName();

  if(machineStateClientAppName.empty() ||
     machineStateClientAppName == newMachineClient)
  {
    machineStateClientAppName = newMachineClient;
    context.getResults().setResult(StateError::K_SUCCESS);

    cout << "State Machine Client \""
        << machineStateClientAppName
        << "\" registered" << endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);

    cout << "State Machine Client \""
         << machineStateClientAppName
         << "\" registration failed" << endl;
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::getMachineState(GetMachineStateContext context)
{
  cout << "getMachineState request received" << endl;

  context.getResults().setState(machineState);

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::setMachineState(SetMachineStateContext context)
{
  std::string state = context.getParams().getState().cStr();

  if(!state.empty() && state != machineState)
  {
    machineState = state;

    context.getResults().setResult(StateError::K_SUCCESS);

    cout << "Machine state changed successfully to "
         << "\"" << machineState << "\"" << endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    cout << "Invalid machine state received - "
         << "\"" << machineState << "\"" << endl;
  }

  return kj::READY_NOW;
}

int ExecutionManager::start()
{
  try
  {
    capnp::EzRpcServer server(kj::heap<ExecutionManager>(),
                              "unix:/tmp/execution_management");
    auto& waitScope = server.getWaitScope();

    cout << "Execution Manager started.." << endl;

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception& e)
  {
    std::cerr << e.getDescription().cStr() << endl;
  }

  return EXIT_SUCCESS;
}

} // namespace ExecutionManager