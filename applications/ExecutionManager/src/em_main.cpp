#include "application_handler.hpp"
#include "execution_manager_client.hpp"
#include "execution_manager.hpp"
#include "execution_manager_server.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <memory>
#include <capnp/rpc-twoparty.h>

int main(int argc, char **argv)
{
  const char* emAddress = "/tmp/execution_management";
  const char* msmAddress = "/tmp/machine_management";
  std::string protocol{"unix:"};

  try
  {
    ::unlink(emAddress);
    auto io = kj::setupAsyncIo();   

    ExecutionManager::ExecutionManager executionManager
      (std::make_unique<ExecutionManager::ManifestReader>(),
          std::make_unique<ExecutionManager::ApplicationHandler>(),
       std::make_unique<ExecutionManagerClient::ExecutionManagerClient>
        (protocol + msmAddress, io));

    capnp::TwoPartyServer server(
      kj::heap<ExecutionManagerServer::ExecutionManagerServer>
      (executionManager));

    auto address = io.provider->getNetwork()
        .parseAddress(protocol + emAddress).wait(io.waitScope);

    auto listener = address->listen();
    auto listenPromise = server.listen(*listener);

    server.drain().wait(io.waitScope);

    listenPromise.wait(io.waitScope);

  }
  catch (const kj::Exception& exception)
  {
    std::cerr << exception.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
}
