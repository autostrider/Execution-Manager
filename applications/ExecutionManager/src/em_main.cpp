#include "execution_manager_server.hpp"
#include "execution_manager.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char **argv)
{
  auto executionManager = ExecutionManager::ExecutionManager(
    std::make_unique<ExecutionManager::ManifestReader>()
  );

  try
  {
    capnp::EzRpcServer server(
      kj::heap<ExecutionManagerServer::ExecutionManagerServer>
      (executionManager),
      "unix:/tmp/execution_management");

    std::thread([&]()
    {
      executionManager.start();
    }).detach();

    auto &waitScope = server.getWaitScope();

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    std::cerr << e.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
}
