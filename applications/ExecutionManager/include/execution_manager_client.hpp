#ifndef EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_CLIENT_HPP

#include <i_execution_manager_client.hpp>

#include <string>
#include <kj/async-io.h>


namespace ExecutionManagerClient
{

class ExecutionManagerClient : public api::IExecutionManagerClient
{
public:
  ExecutionManagerClient(const std::string& msmAddress, 
                         const std::string& componentAddress,
                         kj::AsyncIoContext& context);
                         
  void confirm(StateError status);

  pComponentClientReturnType SetComponentState(std::string& state ,std::string& componentName);

  ~ExecutionManagerClient() override = default;

private:
  const std::string m_msmAddress;
  const std::string m_componentAddress;
  kj::AsyncIoContext& m_ioContext;
  kj::AsyncIoProvider::PipeThread m_sendThread;
};

} // namespace ExecutionManagerClient

#endif // EXECUTION_MANAGER_CLIENT_HPP
