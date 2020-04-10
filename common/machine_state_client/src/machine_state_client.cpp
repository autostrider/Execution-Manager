#include "machine_state_client.h"
#include <client_socket.hpp>
#include <logger.hpp>

#include <any.pb.h>

#include <chrono>
#include <thread>

using namespace std::chrono;

namespace machine_state_client
{

MachineStateClient::MachineStateClient(const std::string& path)
    : m_path{path},
      m_pid(getpid())
{}

void MachineStateClient::setClient(std::unique_ptr<IClient> client)
{
  m_client = std::move(client);
  m_client->connect();
}

StateError MachineStateClient::Register(std::string appName, uint32_t timeout)
{
  MachineStateManagement::registerApp context;
  context.set_appname(appName);
  context.set_pid(m_pid);

  m_client->sendMessage(context);

  return waitForConfirm(timeout);
}

StateError MachineStateClient::GetMachineState(std::string& state, uint32_t timeout)
{
  MachineStateManagement::getMachineState context;
  context.set_pid(m_pid);

  m_client->sendMessage(context);

  return waitForConfirm(state, timeout);
}

StateError MachineStateClient::SetMachineState(std::string state, uint32_t timeout)
{
  MachineStateManagement::setMachineState context;
  context.set_state(state);
  context.set_pid(m_pid);

  m_client->sendMessage(context);

  return waitForConfirm(timeout);
}

StateError MachineStateClient::waitForConfirm(uint32_t timeout)
{
  google::protobuf::Any any;
  std::string data;
  auto start = std::chrono::high_resolution_clock::now();

  do
  {
    if (m_client->receive(data) > 0)
    {
      any.ParseFromString(data);
      
      if (any.Is<MachineStateManagement::resultRegisterApp>())
      {
        MachineStateManagement::resultRegisterApp context;
        any.UnpackTo(&context);
        StateError result = context.result();
        context = {};

        return result;
      }
      else if (any.Is<MachineStateManagement::resultSetMachineState>())
      {
        MachineStateManagement::resultSetMachineState context;
        any.UnpackTo(&context);
        StateError result = context.result();
        context = {};

        return result;
      }
    }
  } while (duration_cast<microseconds>(high_resolution_clock::now() - start).count() < timeout);
  
  return StateError::kTimeout;
}

StateError MachineStateClient::waitForConfirm(std::string& state, uint32_t timeout)
{
  google::protobuf::Any any;
  std::string data;
  auto start = std::chrono::high_resolution_clock::now();

  do
  {    
    if (m_client->receive(data) > 0)
    {
      any.ParseFromString(data);
        
      if(any.Is<MachineStateManagement::resultGetMachineState>())
      {
        MachineStateManagement::resultGetMachineState context;
        any.UnpackTo(&context);

        StateError result = context.result();
        state = context.state();
        context = {};
        
        return result;
      }
    }
  } while (duration_cast<microseconds>(high_resolution_clock::now() - start).count() < timeout);

  return StateError::kTimeout;
}

} // namespace machine_state_client
