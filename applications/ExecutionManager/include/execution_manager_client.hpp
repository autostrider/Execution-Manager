#ifndef EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_CLIENT_HPP

#include <i_execution_manager_client.hpp>

#include <string>

using namespace api;

namespace ExecutionManagerClient
{

class ExecutionManagerClient : public IExecutionManagerClient
{
public:
  ExecutionManagerClient(const std::string& msmAddress, 
                         const std::string& m_componentAddress);
                         
  void confirm(StateError status);

  enums::ComponentClientReturnType SetComponentState(std::string& state ,std::string& componentName);

  ~ExecutionManagerClient() override = default;
private:
  const std::string m_msmAddress;
  const std::string m_componentAddress;
};

} // namespace ExecutionManagerClient

#endif // EXECUTION_MANAGER_CLIENT_HPP
