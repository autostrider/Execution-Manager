#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

#include "machine_state_manager.hpp"
#include "msm_state_machine.hpp"

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main(int argc, char **argv)
{
  if (::signal(SIGTERM, signalHandler) == SIG_ERR)
  {
    std::cout << "[ MachineStateManager ]:\tError while registering signal." << std::endl;
  }
  
  MachineStateManager::MachineStateManager msm(isTerminating);

  while (true)
  {
    msm.transitToNextState();
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
  
  return 0;
}

static void signalHandler(int signo)
{
    std::cout << "[ MachineStateManager ]:\tReceived signal: "
              << sys_siglist[signo] << "." << std::endl;
    isTerminating = true;
}