#ifndef MACHINE_STATE_CLIENT_H
#define MACHINE_STATE_CLIENT_H

#include <client.hpp>

#include <string>

#include <machine_state_management.pb.h>

using namespace base_client;

namespace machine_state_client
{

using StateError = MachineStateManagement::StateError;

class MachineStateClient
{
public:
  MachineStateClient(std::string path);
  ~MachineStateClient();

  StateError Register(std::string appName);
  StateError GetMachineState(std::string& state);
  StateError SetMachineState(std::string state);

private:
  StateError waitForConfirm(google::protobuf::Any any);
  StateError waitForConfirm(google::protobuf::Any any, std::string& state);

private:
  Client m_client;
  pid_t m_pid;
};

}
#endif // MACHINE_STATE_CLIENT_H
