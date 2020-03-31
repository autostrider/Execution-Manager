#include "execution_manager_client.hpp"
#include <client_socket.hpp>
#include <client.hpp>

#include <capnp/rpc-twoparty.h>
#include <execution_management_p.pb.h>

using namespace socket_handler;
using namespace base_client;
using namespace pExecutionManagement;

namespace ExecutionManagerClient
{

ExecutionManagerClient::ExecutionManagerClient(const std::string& msmAddress,
                                               const std::string& componentAddress,
                                               kj::AsyncIoContext& context)
: m_msmAddress(msmAddress),
  m_componentAddress(componentAddress),
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

pComponentClientReturnType
ExecutionManagerClient::SetComponentState(std::string& state, 
                                          std::string& componentName)
{  
  auto m_client =
      std::make_unique<Client>((m_componentAddress + componentName),
                                std::move(std::make_unique<ClientSocket>()));
  m_client->connect();

  pSetCompState message;
  message.set_state(state);

  google::protobuf::Any any;
  any.PackFrom(message);

  m_client->sendMessage(any);

  return pComponentClientReturnType::kSuccess;
}

} // namespace ExecutionManagerClient
