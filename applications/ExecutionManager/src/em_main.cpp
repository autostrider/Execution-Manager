#include "application_handler.hpp"
#include "execution_manager_server.hpp"
#include "manifest_reader.hpp"
#include "os_interface.hpp"
#include <iostream>
#include <memory>

int main(int argc, char **argv)
{
  try
  {
    ::unlink(EM_SOCKET_NAME.c_str());
    auto io = kj::setupAsyncIo();

    ExecutionManager::ExecutionManager executionManager
      (std::make_unique<ExecutionManager::ManifestReader>(),
          std::make_unique<ExecutionManager::ApplicationHandler>(
           std::make_unique<ExecutionManager::OsInterface>()
           ),
       std::make_unique<ExecutionManagerClient::ExecutionManagerClient>
        (IPC_PROTOCOL + MSM_SOCKET_NAME, io)
       );

    capnp::TwoPartyServer server(
      kj::heap<ExecutionManagerServer::ExecutionManagerServer>
      (executionManager));

    auto address = io.provider->getNetwork()
        .parseAddress(IPC_PROTOCOL + EM_SOCKET_NAME).wait(io.waitScope);

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
