#ifndef EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_CLIENT_HPP

#include <i_execution_manager_client.hpp>
#include <string>
#include <kj/async-io.h>

namespace ExecutionManagerClient
{

class ExecutionManagerClient : public IExecutionManagerClient
{
public:
  ExecutionManagerClient(const std::string& msmAddress, 
                         kj::AsyncIoContext& context, 
                         std::string m_componentAddress);
                         
  void confirm(StateError status);

  StateManagement::ComponentClientReturnType SetComponentState(std::string& state ,std::string& componentName);

  ~ExecutionManagerClient() override = default;
private:
  const std::string m_msmAddress;
  kj::AsyncIoContext& m_ioContext;

  const std::string m_componentAddress;

  kj::AsyncIoProvider::PipeThread m_sendThread;
};

} // namespace ExecutionManagerClient

#endif // EXECUTION_MANAGER_CLIENT_HPP
