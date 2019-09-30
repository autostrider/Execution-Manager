#include "execution_manager_client.hpp"

#include <capnp/rpc-twoparty.h>

namespace ExecutionManagerClient
{

ExecutionManagerClient::ExecutionManagerClient(const std::string& msmAddress,
  kj::AsyncIoContext& context)
: m_msmAddress(msmAddress),
  m_ioContext(context)
{}

void
ExecutionManagerClient::confirm(StateError status)
{
  m_sendThread = m_ioContext.provider->newPipeThread(
    [&, status](kj::AsyncIoProvider& ioProvider,
        auto&,
        kj::WaitScope& waitScope)
  {
    auto address = ioProvider.getNetwork()
      .parseAddress(m_msmAddress)
        .wait(waitScope);

    auto connection = address->connect().wait(waitScope);
    capnp::TwoPartyClient client(*connection);
    auto capability = client.bootstrap()
      .castAs<MachineStateManagement::MachineStateManager>();

    auto request = capability.confirmStateTransitionRequest();

    request.setResult(status);

    request.send().ignoreResult().wait(waitScope);
  });
}

} // namespace ExecutionManagerClient
