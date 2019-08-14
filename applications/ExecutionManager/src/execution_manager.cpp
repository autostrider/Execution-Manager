#include "execution_manager.hpp"

namespace ExecutionManager {

::kj::Promise<void> 
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();
  
  std::cout << "State #" << static_cast<uint16_t>(state) 
            << " received" 
            << std::endl;

  return kj::READY_NOW;
}

int ExecutionManager::start()
{
  capnp::EzRpcServer server(kj::heap<ExecutionManager>(), 
                            "unix:/tmp/execution_management", 5923);
  auto& waitScope = server.getWaitScope();
  
  std::cout << "Execution Manager started.." << std::endl;

  kj::NEVER_DONE.wait(waitScope);

  return EXIT_SUCCESS;
}

} // namespace ExecutionManager