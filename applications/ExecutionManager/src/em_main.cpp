#include "execution_manager.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <memory>
#include <unistd.h>

int main(int argc, char **argv)
{
  try
  {
    ::unlink("/tmp/execution_management");
    capnp::EzRpcServer server(kj::heap<ExecutionManager::ExecutionManager>(
                                std::make_unique<ExecutionManager::ManifestReader>()),
                              "unix:/tmp/execution_management");
    auto &waitScope = server.getWaitScope();

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    std::cerr << e.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
}
