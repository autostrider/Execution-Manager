#include "execution_manager_client.hpp"

#include <capnp/rpc-twoparty.h>
#include <logger.hpp>
namespace ExecutionManagerClient
{

ExecutionManagerClient::ExecutionManagerClient(const std::string& msmAddress,
                                               const std::string& m_componentAddress,
                                               kj::AsyncIoContext& context)
: m_msmAddress(msmAddress),
  m_componentAddress(m_componentAddress),
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

StateManagement::ComponentClientReturnType 
ExecutionManagerClient::SetComponentState(std::string& state, 
                                          std::string& componentName)
{
  m_ioContext.provider->newPipeThread(
    [&](kj::AsyncIoProvider& ioProvider,
        auto&, 
        kj::WaitScope& waitScope)
  {
    auto address = ioProvider.getNetwork()
      .parseAddress(m_componentAddress + componentName)
        .wait(waitScope);

    auto connection = address->connect().wait(waitScope);
    capnp::TwoPartyClient client(*connection);
    auto capability = client.bootstrap()
      .castAs<StateManagement::StateManager>();

    auto request = capability.setComponentStateRequest();

    request.setState(state);

    request.send().ignoreResult().wait(waitScope);

  });

  return StateManagement::ComponentClientReturnType::K_SUCCESS;
}

} // namespace ExecutionManagerClient
