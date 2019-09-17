#include <iostream>
#include "machine_state_manager.hpp"

int main(int argc, char **argv)
{
  const char* socketName = "/tmp/machine_management";
  ::unlink(socketName);

  MachineStateManager::MachineStateManager msm;
  msm.start();
  return EXIT_SUCCESS;
}
