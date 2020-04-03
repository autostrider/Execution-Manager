#include "machine_state_client.h"
#include <client_socket.hpp>
#include <machine_state_management.pb.h>

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

  google::protobuf::Any sendData;
  google::protobuf::Any recvData;

  sendData.PackFrom(context);

  m_client.sendMessage(sendData);

  return waitForConfirm(recvData);
}

StateError MachineStateClient::GetMachineState(std::string& state)
{
  MachineStateManagement::getMachineState context;
  context.set_pid(m_pid);

  google::protobuf::Any sendData;
  google::protobuf::Any recvData;

  sendData.PackFrom(context);

  m_client.sendMessage(sendData);

  return waitForConfirm(recvData, state);
}

StateError MachineStateClient::SetMachineState(std::string state)
{
  MachineStateManagement::setMachineState context;
  context.set_state(state);
  context.set_pid(m_pid);

  google::protobuf::Any sendData;
  google::protobuf::Any recvData;

  sendData.PackFrom(context);

  m_client.sendMessage(sendData);

  return waitForConfirm(recvData);
}

StateError MachineStateClient::waitForConfirm(google::protobuf::Any any)
{
  while(m_client.getRecvBytes() == -1)
  {
      auto data = m_client.receive();
      any.ParseFromString(data);

      if(any.Is<MachineStateManagement::resultRegisterApp>())
      {
          MachineStateManagement::resultRegisterApp context;
          any.UnpackTo(&context);
          StateError result = context.result();
          context = {};
          return result;
      }
      else if(any.Is<MachineStateManagement::resultSetMachineState>())
      {
          MachineStateManagement::resultSetMachineState context;
          any.UnpackTo(&context);
          StateError result = context.result();
          context = {};
          return result;
      }
  }
}

StateError MachineStateClient::waitForConfirm(google::protobuf::Any any, std::string& state)
{
  while(m_client.getRecvBytes() == -1)
  {
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
}

} // namespace machine_state_client