#ifndef EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_CLIENT_HPP

#include <string>
#include <capnp/ez-rpc.h>
#include <capnp/rpc-twoparty.h>
#include <kj/async-io.h>
#include <execution_management.capnp.h>

namespace ExecutionManagerClient
{

using StateError = ::MachineStateManagement::StateError;

class ExecutionManagerClient
{
public:
  ExecutionManagerClient(const std::string& msmAddress, kj::AsyncIoContext& context);
  void confirm(StateError status);

private:
  const std::string m_msmAddress;
  kj::AsyncIoContext& m_ioContext;

  kj::AsyncIoProvider::PipeThread m_sendThread;
};

} // namespace ExecutionManagerClient

#endif // EXECUTION_MANAGER_CLIENT_HPP
