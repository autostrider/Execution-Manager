#include <iostream>
#include "machine_state_manager.hpp"

int main(int argc, char **argv)
{
  MachineStateManager::MachineStateManager msm;

  return msm.start();
}
