#include "application_handler.hpp"
#include "execution_manager_server.hpp"
#include "manifest_reader.hpp"
#include "execution_manager_client.hpp"
#include "os_interface.hpp"
#include <logger.hpp>

#include <cerrno>
#include <csignal>
#include <capnp/rpc-twoparty.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char **argv)
{
  if (::signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    LOG << "SIGCHILD was ignored.";
  }
  try
  {
    if ( my_chmod("../applications/ExecutionManager/machine_manifest.json", S_IRUSR|S_IRGRP|S_IROTH) !=0)
    {
      LOG << strerror(errno);
    }
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
      (executionManager, ExecutionManager::MsmHandler{}));

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

static inline int my_chmod(const char * path, mode_t mode)
{
    int result = chmod(path, mode);

    if (result != 0)
    {
        result = errno;
    }

    return (result);
}
