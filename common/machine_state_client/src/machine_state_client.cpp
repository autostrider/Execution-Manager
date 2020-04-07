#include "machine_state_client.h"
#include <client_socket.hpp>
#include <machine_state_management.pb.h>
#include <any.pb.h>

#include <thread>
#include <chrono>

namespace machine_state_client
{

MachineStateClient::MachineStateClient(std::string path)
    : m_client(path, std::make_unique<socket_handler::ClientSocket>()),
      m_pid(getpid())
{
  m_client.connect();
}

MachineStateClient::~MachineStateClient()
{
  m_client.~Client();
}

StateError MachineStateClient::Register(std::string appName)
{
  MachineStateManagement::registerApp context;
  context.set_appname(appName);
  context.set_pid(m_pid);

  m_client.sendMessage(context);

  return waitForConfirm();
}

StateError MachineStateClient::GetMachineState(std::string& state)
{
  MachineStateManagement::getMachineState context;
  context.set_pid(m_pid);

  m_client.sendMessage(context);

  return waitForConfirm(state);
}

StateError MachineStateClient::SetMachineState(std::string state)
{
  MachineStateManagement::setMachineState context;
  context.set_state(state);
  context.set_pid(m_pid);

  m_client.sendMessage(context);

  return waitForConfirm();
}

StateError MachineStateClient::waitForConfirm()
{
  google::protobuf::Any any;
  std::string data;

  do
  {
    if (m_client.receive(data) > 0)
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
        
        any = {};
        data = {};
    }
  } while (true);
}

StateError MachineStateClient::waitForConfirm(std::string& state)
{
  google::protobuf::Any any;
  std::string data;

  do
  {    
    if (m_client.receive(data) > 0)
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

      any = {};
      data = {};
    }
  } while (true);
}

} // namespace machine_state_client