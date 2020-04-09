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
  MachineStateClient(const std::string& path);
  ~MachineStateClient() = default;

  StateError Register(std::string appName, uint32_t timeout);
  StateError GetMachineState(std::string& state, uint32_t timeout);
  StateError SetMachineState(std::string state, uint32_t timeout);

protected:
  void setClient(std::unique_ptr<IClient> client);

private:
  StateError waitForConfirm(uint32_t timeout);
  StateError waitForConfirm(std::string& state, uint32_t timeout);

private:
  std::unique_ptr<IClient> m_client;
  pid_t m_pid;
  const std::string m_path;
};

}
#endif // MACHINE_STATE_CLIENT_H
