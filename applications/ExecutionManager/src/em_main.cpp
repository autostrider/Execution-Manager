#include "execution_manager.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <memory>

int main(int argc, char **argv)
{
  try
  {
    capnp::EzRpcServer server(kj::heap<ExecutionManager::ExecutionManager>(
                                std::make_unique<ExecutionManager::ManifestReader>()),
                              "unix:/tmp/execution_management");
    auto &waitScope = server.getWaitScope();

    std::cout << "Execution Manager started.." << std::endl;

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    std::cerr << e.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
}
