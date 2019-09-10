#include "execution_manager_server.hpp"
#include "execution_manager.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char **argv)
{
  const char* socketName = "/tmp/execution_management";
  auto executionManager = ExecutionManager::ExecutionManager(
    std::make_unique<ExecutionManager::ManifestReader>()
  );

  try
  {
    ::unlink(socketName);
    capnp::EzRpcServer server(
      kj::heap<ExecutionManagerServer::ExecutionManagerServer>
      (executionManager),
      std::string{"unix:"} + socketName);

    auto &waitScope = server.getWaitScope();

    server.getPort().then([&](capnp::uint port)
    {
      executionManager.start();
    }).wait(waitScope);

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    std::cerr << e.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
}
